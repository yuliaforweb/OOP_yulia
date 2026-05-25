import { TaskManager } from "./taskManager.js";

// Простий фреймворк для тестування без зовнішніх залежностей
function runTest(testName, testFn) {
    try {
        testFn();
        console.log(`[PASS] ${testName}`);
    } catch (error) {
        console.error(`[FAIL] ${testName}`);
        console.error(error.message);
    }
}

const manager = new TaskManager();

// Тест створення задачі та роботи методів класу Task
runTest("Task creation and methods verification", () => {
    const task = manager.createTask({
        id: 1,
        title: "Підготувати практичну №5",
        description: "Написати код менеджеру задач на JS",
        priority: "high",
        dueDate: "2026-06-01",
        tags: ["навчання", "js"]
    });

    if (task.status !== "pending") throw new Error("Статус за замовчуванням має бути pending");
    
    task.markComplete();
    if (task.status !== "completed") throw new Error("Метод markComplete не змінив статус");

    task.markIncomplete();
    if (task.status !== "pending") throw new Error("Метод markIncomplete не повернув статус pending");

    task.addTag("важливо");
    if (!task.tags.includes("важливо")) throw new Error("Тег не додався");

    task.removeTag("js");
    if (task.tags.includes("js")) throw new Error("Тег не видалився");
});

// Тест перевірки геттера isOverdue
runTest("Task isOverdue getter validation", () => {
    const overdueTask = manager.createTask({
        id: 2,
        title: "Стара лабораторна",
        description: "Яку не встигла здати вчасно",
        priority: "medium",
        dueDate: "2026-01-01",
        tags: ["архів"]
    });

    if (!overdueTask.isOverdue) throw new Error("Геттер isOverdue мав повернути true для минулої дати");
});

// Тест оновлення та видалення задач в TaskManager
runTest("TaskManager CRUD operations (Update/Delete)", () => {
    manager.updateTask(1, { title: "Оновлена назва практичної", priority: "low" });
    const task = manager.tasks.find(t => t.id === 1);
    
    if (task.title !== "Оновлена назва практичної" || task.priority !== "low") {
        throw new Error("Дані задачі не оновилися через updateTask");
    }

    const deleteResult = manager.deleteTask(1);
    if (!deleteResult || manager.tasks.find(t => t.id === 1)) {
        throw new Error("Задача не видалилася через deleteTask");
    }
});

// Тест методів фільтрації та пошуку
runTest("TaskManager Filtering and Search methods", () => {
    manager.createTask({
        id: 3,
        title: "Вивчити React",
        description: "Переглянути туторіал по компонентах",
        priority: "high",
        dueDate: "2026-07-15",
        tags: ["веб", "фронтенд"]
    });

    const highPriorityTasks = manager.getTasksByPriority("high");
    if (highPriorityTasks.length === 0) throw new Error("Фільтрація за пріоритетом не працює");

    const searchResult = manager.searchTasks("React");
    if (searchResult.length === 0) throw new Error("Пошук за ключовим словом не знайшов задачу");
});

// Тест розрахунку статистики та групування
runTest("Statistics and Object Transformation methods", () => {
    const stats = manager.getStats();
    if (typeof stats.total !== "number" || !("completed" in stats)) {
        throw new Error("Метод getStats повернув некоректну структуру");
    }

    const rate = manager.getCompletionRate();
    if (typeof rate !== "number") throw new Error("getCompletionRate має повертати число");

    const grouped = manager.getTasksByDueDate();
    const keys = Object.keys(grouped);
    if (keys.length === 0) throw new Error("Групування за датами виконання повернуло порожній об'єкт");
});

console.log("Усі тести виконано.");