/**
 * JSDoc для представлення структури задачі
 * @class Task
 */
export class Task {
    /**
     * Створення нової задачі за допомогою деструктуризації об'єкта даних
     * @param {Object} param0 - Об'єкт з параметрами задачі
     */
    constructor({ id, title, description, status = "pending", priority, dueDate, tags = [] }) {
        this.id = id;
        this.title = title;
        this.description = description;
        this.status = status;
        this.priority = priority;
        this.dueDate = new Date(dueDate);
        this.tags = tags;
    }

    /**
     * Зміна статусу задачі на виконану
     */
    markComplete() {
        this.status = "completed";
    }

    /**
     * Зміна статусу задачі на невиконану
     */
    markIncomplete() {
        this.status = "pending";
    }

    /**
     * Додавання нового тегу до задачі
     * @param {string} tag
     */
    addTag(tag) {
        if (!this.tags.includes(tag)) {
            this.tags.push(tag);
        }
    }

    /**
     * Видалення тегу із задачі
     * @param {string} tag
     */
    removeTag(tag) {
        this.tags = this.tags.filter(t => t !== tag);
    }

    /**
     * Геттер для перевірки, чи протермінована задача
     * @returns {boolean}
     */
    get isOverdue() {
        if (this.status === "completed") {
            return false;
        }

        const today = new Date();
        today.setHours(0, 0, 0, 0);

        const deadline = new Date(this.dueDate);
        deadline.setHours(0, 0, 0, 0);

        return today > deadline;
    }
}