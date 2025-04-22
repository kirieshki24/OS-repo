import threading

# Глобальная блокировка для многопоточного доступа
_lock = threading.RLock()
# Словарь синглтонов
_instances = {}

def singleton(cls):
    """
    Декоратор для создания синглтона.
    Реализует паттерн Double-Checked Locking для безопасного доступа из разных потоков.
    """
    def get_instance():
        if cls not in _instances:
            with _lock:
                if cls not in _instances:
                    _instances[cls] = cls()
        return _instances[cls]
    
    cls.instance = staticmethod(get_instance)
    return cls

@singleton
class FactorialCalculator:
    """Класс для вычисления факториала числа."""
    
    def __init__(self):
        self.cache = {0: 1, 1: 1}
        self.calls_count = 0
        print(f"FactorialCalculator создан: {id(self)}")
    
    def factorial(self, n: int) -> int:
        """Вычисление факториала с кешированием результатов."""
        self.calls_count += 1
        
        # Возвращаем из кеша, если значение уже вычислено
        if n in self.cache:
            return self.cache[n]
        
        # Вычисляем рекурсивно
        if n < 0:
            raise ValueError("Факториал определен только для неотрицательных чисел")
        
        result = n * self.factorial(n - 1)
        self.cache[n] = result
        return result
    
    def get_stats(self):
        """Получение статистики использования калькулятора."""
        return {
            "calls_count": self.calls_count,
            "cache_size": len(self.cache),
            "cache": self.cache
        }
    
    def clear_cache(self):
        """Очистка кеша, кроме базовых значений."""
        self.cache = {0: 1, 1: 1}

def example_usage():
    """Демонстрация использования синглтона для вычисления факториала."""
    
    print("=== Демонстрация работы синглтона ===")
    
    # Получаем экземпляр синглтона
    calc1 = FactorialCalculator.instance()
    
    # Вычисляем факториалы
    print(f"Факториал 5: {calc1.factorial(5)}")
    print(f"Факториал 10: {calc1.factorial(10)}")
    
    # Получаем ещё один экземпляр синглтона (должен быть тем же самым)
    calc2 = FactorialCalculator.instance()
    
    # Проверяем, что это один и тот же объект
    print(f"Это один и тот же объект? {calc1 is calc2}")
    
    # Вычисляем ещё один факториал
    print(f"Факториал 7: {calc2.factorial(7)}")
    
    # Получаем статистику использования
    print(f"Статистика использования: {calc1.get_stats()}")
    
    # Многопоточное использование
    def worker(numbers, results):
        """Функция рабочего потока."""
        calculator = FactorialCalculator.instance()
        for num in numbers:
            results[num] = calculator.factorial(num)
    
    # Создаем потоки
    results = {}
    threads = []
    
    thread1 = threading.Thread(target=worker, args=([3, 8, 12], results))
    thread2 = threading.Thread(target=worker, args=([4, 9, 11], results))
    
    threads.append(thread1)
    threads.append(thread2)
    
    # Запускаем потоки
    for thread in threads:
        thread.start()
    
    # Ждем завершения
    for thread in threads:
        thread.join()
    
    # Выводим результаты и итоговую статистику
    print(f"Результаты вычислений из потоков: {results}")
    print(f"Итоговая статистика: {FactorialCalculator.instance().get_stats()}")

if __name__ == "__main__":
    example_usage()