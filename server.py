import paho.mqtt.client as mqtt
import threading
import signal
from datetime import datetime

class RobotServer:
    def __init__(self):
        # Конфигурация подключения
        self.BROKER_IP = "192.168.0.101"
        self.BROKER_PORT = 1883
        self.TOPICS = {
            'input': "laptop/commands",
            'output': "robot/commands"
        }
        
        # Инициализация клиента MQTT
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.stop_timer = None
        self.running = False
        self._setup_handlers()

    def _setup_handlers(self):
        """Настройка обработчиков событий MQTT"""
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message

    def _log(self, message, level="INFO"):
        """Визуализация логов с временными метками"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] {level}: {message}")

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """Обработчик успешного подключения"""
        self._log(f"Успешное подключение к брокеру {self.BROKER_IP}")
        client.subscribe(self.TOPICS['input'])
        self._log(f"Подписка на топик '{self.TOPICS['input']}'")

    def _on_message(self, client, userdata, msg):
        """Обработчик входящих сообщений"""
        try:
            payload = msg.payload.decode().strip()
            if not payload:
                self._log("Пустая полезная нагрузка", "WARN")
                return

            self._process_command(payload)

        except Exception as e:
            self._log(f"Ошибка обработки сообщения: {str(e)}", "ERROR")

    def _process_command(self, payload):
        """Анализ и выполнение полученной команды"""
        if payload.lower() == "shutdown":
            self.shutdown()
            return

        if ':' not in payload:
            self._log(f"Некорректный формат команды: {payload}", "WARN")
            return

        command_part, duration_part = payload.split(':', 1)
        command = command_part.strip().lower()
        duration = duration_part.strip()

        if not self._validate_duration(duration):
            return

        self._execute_command(command, int(duration))

    def _validate_duration(self, duration_str):
        """Проверка корректности длительности"""
        try:
            duration = int(duration_str)
            if duration < 0:
                raise ValueError
            return True
        except ValueError:
            self._log(f"Некорректная длительность: {duration_str}", "WARN")
            return False

    def _execute_command(self, command, duration):
        """Выполнение валидной команды"""
        self._log(f"Получена команда: {command} ({duration}мс)")
        self._publish_command(command)

        if duration > 0:
            if self.stop_timer:
                self.stop_timer.cancel()
            
            self.stop_timer = threading.Timer(
                duration / 1000,
                lambda: self._publish_command('stop')
            )
            self.stop_timer.start()
            self._log(f"Таймер остановки установлен на {duration}мс")

    def _publish_command(self, command):
        """Публикация команды для робота"""
        try:
            self.client.publish(self.TOPICS['output'], command)
            self._log(f"Отправлена команда: {command}")
        except Exception as e:
            self._log(f"Ошибка отправки: {str(e)}", "ERROR")

    def start(self):
        """Запуск MQTT-клиента"""
        try:
            self.client.connect(self.BROKER_IP, self.BROKER_PORT, 60)
            self.running = True
            
            signal.signal(signal.SIGINT, self._graceful_exit)
            signal.signal(signal.SIGTERM, self._graceful_exit)
            
            self._log("Сервер успешно запущен")
            
            while self.running:
                self.client.loop(timeout=1.0)

        except Exception as e:
            self._log(f"Критическая ошибка: {str(e)}", "ERROR")
        finally:
            self._cleanup_resources()

    def shutdown(self):
        """Инициирование процедуры выключения"""
        self._log("Инициировано выключение системы...", "WARN")
        self._publish_command("shutdown")
        self.running = False

    def _graceful_exit(self, signum, frame):
        """Обработчик сигналов завершения работы"""
        self._log(f"Получен сигнал {signal.Signals(signum).name}", "WARN")
        self.shutdown()

    def _cleanup_resources(self):
        """Освобождение ресурсов"""
        if self.stop_timer:
            self.stop_timer.cancel()
        self.client.disconnect()
        self._log("Все ресурсы освобождены")

if __name__ == "__main__":
    server = RobotServer()
    server.start()
