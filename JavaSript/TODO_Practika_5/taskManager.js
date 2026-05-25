import { Task } from "./task.js";

/**
 * JSDoc для управління колекцією задач
 * @class TaskManager
 */
export class TaskManager {
    constructor() {
        this.tasks = [];
    }

    /**
     * Створення та додавання задачі за допомогою spread оператора
     * @param {Object} data 
     * @returns {Task}
     */
    createTask(data) {
        const task = new Task({ ...data });
        this.tasks.push(task);
        return task;
    }

    /**
     * Оновлення задачі за допомогою Object.entries
     * @param {number|string} id 
     * @param {Object} data 
     * @returns {Task|null}
     */
    updateTask(id, data) {
        const task = this.tasks.find(t => t.id === id);
        if (!task) return null;

        const updatedData = { ...data };

        // Використання Object.entries для динамічного оновлення властивостей
        Object.entries(updatedData).forEach(([key, value]) => {
            if (key === "dueDate") {
                task.dueDate = new Date(value);
            } else if (key !== "id") {
                task[key] = value;
            }
        });

        return task;
    }

    /**
     * Видалення задачі за id
     * @param {number|string} id 
     * @returns {boolean}
     */
    deleteTask(id) {
        const index = this.tasks.findIndex(t => t.id === id);
        if (index !== -1) {
            this.tasks.splice(index, 1);
            return true;
        }
        return false;
    }

    getTasksByStatus(status) {
        return this.tasks.filter(t => t.status === status);
    }

    getTasksByPriority(priority) {
        return this.tasks.filter(t => t.priority === priority);
    }

    getOverdueTasks() {
        return this.tasks.filter(t => t.isOverdue);
    }

    getTasksByTag(tag) {
        return this.tasks.filter(t => t.tags.includes(tag));
    }

    searchTasks(query) {
        const lowerQuery = query.toLowerCase();
        return this.tasks.filter(t => 
            t.title.toLowerCase().includes(lowerQuery) || 
            t.description.toLowerCase().includes(lowerQuery)
        );
    }

    /**
     * Отримання загальної статистики задач
     * @returns {Object}
     */
    getStats() {
        return {
            total: this.tasks.length,
            completed: this.getTasksByStatus("completed").length,
            pending: this.getTasksByStatus("pending").length,
            overdue: this.getOverdueTasks().length
        };
    }

    /**
     * Розрахунок відсотка виконання задач
     * @returns {number}
     */
    getCompletionRate() {
        const stats = this.getStats();
        if (stats.total === 0) return 0;
        return Math.round((stats.completed / stats.total) * 100);
    }

    /**
     * Групування задач по датах з використанням Object.keys
     * @returns {Object}
     */
    getTasksByDueDate() {
        const grouped = this.tasks.reduce((acc, task) => {
            const dateStr = task.dueDate.toISOString().split("T")[0];
            if (!acc[dateStr]) {
                acc[dateStr] = [];
            }
            acc[dateStr].push(task);
            return acc;
        }, {});

        const sortedGroups = {};
        // Використання Object.keys для сортування ключів-дат за хронологією
        Object.keys(grouped).sort().forEach(key => {
            sortedGroups[key] = grouped[key];
        });

        return sortedGroups;
    }

    /**
     * Додатковий метод для демонстрації роботи з Object.values
     * @returns {number}
     */
    getTotalActiveTasksCount() {
        const activeStats = {
            pending: this.getTasksByStatus("pending").length,
            overdue: this.getOverdueTasks().length
        };
        // Використання Object.values для підсумовування значень
        return Object.values(activeStats).reduce((sum, val) => sum + val, 0);
    }
}