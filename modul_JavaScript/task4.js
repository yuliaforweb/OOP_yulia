// Клас для однієї задачі
class Task {
    constructor(id, text) {
        this.id = id;
        this.text = text;
        this.done = false;
    }

    toggle() {
        this.done = !this.done;
    }
}

// Клас для управління списком задач
class TodoList {
    constructor() {
        this.todoItems = [];
    }

    add(text) {
        let uniqueId = Date.now() + Math.random().toString(36).substr(2, 5);
        let newTask = new Task(uniqueId, text);
        this.todoItems.push(newTask);
        return newTask;
    }

    remove(id) {
        this.todoItems = this.todoItems.filter(item => item.id !== id);
    }

    getActive() {
        return this.todoItems.filter(item => item.done === false);
    }
}

// Робота з DOM елементами сторінки
const myTodoList = new TodoList();

const taskInput = document.getElementById("taskInput") || document.createElement("input");
const addBtn = document.getElementById("addBtn") || document.createElement("button");
const taskList = document.getElementById("taskList") || document.createElement("ul");

// Функція оновлення інтерфейсу
function renderUI() {
    taskList.innerHTML = "";

    for (let task of myTodoList.todoItems) {
        let li = document.createElement("li");
        li.textContent = task.text;
        li.dataset.id = task.id;

        if (task.done) {
            li.style.textDecoration = "line-through";
            li.style.color = "gray";
        }

        taskList.appendChild(li);
    }
}

// Додавання задачі по кліку на кнопку
addBtn.addEventListener("click", () => {
    let text = taskInput.value.trim();
    if (text !== "") {
        myTodoList.add(text);
        taskInput.value = "";
        renderUI();
    }
});

// Перехоплення подій через батьківський елемент
taskList.addEventListener("click", (event) => {
    if (event.target.tagName === "LI") {
        let clickedId = event.target.dataset.id;

        let foundTask = myTodoList.todoItems.find(item => item.id === clickedId);
        if (foundTask) {
            foundTask.toggle();
            renderUI();
        }
    }
});

// Блок автоматичного створення HTML елементів для тестів у консолі браузера
if (!document.getElementById("taskInput") && typeof window !== "undefined") {
    const testContainer = document.createElement("div");
    testContainer.innerHTML = `
        <div style="background: #f0f0f0; padding: 10px; margin-bottom: 10px;">
            <input id="taskInput" placeholder="Нове завдання">
            <button id="addBtn">Додати</button>
            <ul id="taskList"></ul>
        </div>
    `;
    document.body.prepend(testContainer);

    setTimeout(() => {
        const actualInput = document.getElementById("taskInput");
        const actualBtn = document.getElementById("addBtn");
        const actualList = document.getElementById("taskList");

        actualBtn.addEventListener("click", () => {
            let text = actualInput.value.trim();
            if (text !== "") {
                myTodoList.add(text);
                actualInput.value = "";
                actualList.innerHTML = "";
                for (let t of myTodoList.todoItems) {
                    let li = document.createElement("li");
                    li.textContent = t.text;
                    li.dataset.id = t.id;
                    if (t.done) li.style.textDecoration = "line-through";
                    actualList.appendChild(li);
                }
            }
        });

        actualList.addEventListener("click", (e) => {
            if (e.target.tagName === "LI") {
                let id = e.target.dataset.id;
                let t = myTodoList.todoItems.find(item => item.id === id);
                if (t) t.toggle();
                actualList.innerHTML = "";
                for (let taskItem of myTodoList.todoItems) {
                    let li = document.createElement("li");
                    li.textContent = taskItem.text;
                    li.dataset.id = taskItem.id;
                    if (taskItem.done) li.style.textDecoration = "line-through";
                    actualList.appendChild(li);
                }
            }
        });
    }, 100);
}
