import { TaskManager } from "./taskManager.js";

const manager = new TaskManager();

console.log("=== КРОК 1: Створення задач та встановлення дат ===");
const task1 = manager.createTask({
    id: 101,
    title: "Здати практичну роботу з JS",
    description: "Записати відео демки та завантажити код",
    priority: "high",
    dueDate: "2026-05-30",
    tags: ["навчання", "коледж"]
});

const task2 = manager.createTask({
    id: 102,
    title: "Вечеря",
    description: "Приготувати вечерю",
    priority: "medium",
    dueDate: "2026-05-20", // Дата в минулому для перевірки протермінування
    tags: ["побут"]
});

console.log("Поточні задачі в системі:");
console.log(manager.tasks);

console.log("\n=== КРОК 2: Перевірка протермінованих задач ===");
console.log("Знайдено протерміновані задачі (геттер isOverdue):");
console.log(manager.getOverdueTasks());

console.log("\n=== КРОК 3: Зміна задачі (updateTask) ===");
manager.updateTask(101, { title: "Оновлений заголовок практичної №5", priority: "low" });
console.log("Задача після зміни:");
console.log(manager.tasks.find(t => t.id === 101));

console.log("\n=== КРОК 4: Відмітка задачі як виконаної (markComplete) ===");
task1.markComplete();
console.log(`Новий статус задачі 101: ${task1.status}`);
console.log("Поточна статистика:");
console.log(manager.getStats());

console.log("\n=== КРОК 5: Видалення задачі (deleteTask) ===");
const isDeleted = manager.deleteTask(102);
console.log(`Успішність видалення задачі 102: ${isDeleted}`);
console.log("Задачі в системі після видалення:");
console.log(manager.tasks);