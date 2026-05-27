
// 1. Клас Task
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

// 2. Клас TodoList
class TodoList {
    constructor() {
        this.todoItems = [];
    }

    add(text) {
        let uniqueId = Date.now().toString();
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

// 3. Інтеграція з DOM та делегування подій
const myTodoList = new TodoList();

const taskInput = document.getElementById("taskInput");
const addBtn = document.getElementById("addBtn");
const taskList = document.getElementById("taskList");

// Функція для відрендерення списку на сторінці
function renderUI() {
    taskList.innerHTML = "";

    for (let task of myTodoList.todoItems) {
        let li = document.createElement("li");
        li.dataset.id = task.id;

        // Текст завдання
        let textSpan = document.createElement("span");
        textSpan.textContent = task.text;

        if (task.done) {
            textSpan.style.textDecoration = "line-through";
            textSpan.style.color = "gray";
        }

        // Кнопка Виконано
        let doneBtn = document.createElement("button");
        doneBtn.textContent = "Виконано";
        doneBtn.className = "done-btn";

        // Кнопка Видалити
        let removeBtn = document.createElement("button");
        removeBtn.textContent = "Видалити";
        removeBtn.className = "delete-btn";

        li.appendChild(textSpan);
        li.appendChild(doneBtn);
        li.appendChild(removeBtn);
        taskList.appendChild(li);
    }
}

// Обробка кліку на кнопку Додати
addBtn.addEventListener("click", () => {
    let text = taskInput.value.trim();
    if (text !== "") {
        myTodoList.add(text);
        taskInput.value = "";
        renderUI();
    }
});

// Делегування подій на рівні батьківського тегу ul
taskList.addEventListener("click", (event) => {
    // Якщо клікнули по кнопці "Виконано"
    if (event.target.classList.contains("done-btn")) {
        let clickedId = event.target.closest("li").dataset.id;

        let foundTask = myTodoList.todoItems.find(item => item.id === clickedId);
        if (foundTask) {
            foundTask.toggle();
            renderUI();
        }
    }

    // Якщо клікнули по кнопці "Видалити"
    if (event.target.classList.contains("delete-btn")) {
        let clickedId = event.target.closest("li").dataset.id;
        myTodoList.remove(clickedId); // Викликаю метод remove з класу TodoList
        renderUI();
    }
});