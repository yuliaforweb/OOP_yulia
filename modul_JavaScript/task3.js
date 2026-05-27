function createApiClient(baseUrl) {
    // Приватний лічильник у замиканні
    let totalRequestsCount = 0;

    return {
        // Асинхронний метод для fetch запитів
        async get(path) {
            totalRequestsCount++;

            try {
                let response = await fetch(baseUrl + path);

                if (!response.ok) {
                    return { error: "Запит не вдався" };
                }

                return await response.json();
            } catch (networkError) {
                // Обробка помилки мережі
                return { error: "Запит не вдався" };
            }
        },

        // Повернення значення лічильника
        getRequestCount() {
            return totalRequestsCount;
        }
    };
}

// Перевірка асинхронної роботи
async function runDemo() {
    const api = createApiClient("https://jsonplaceholder.typicode.com");

    console.log("--- Запуск асинхронних запитів ---");

    const user = await api.get("/users/5");
    console.log("Ім'я користувача:", user.name);

    const posts = await api.get("/posts?_limit=2");
    console.log("Кількість постів:", posts.length);

    console.log("Всього запитів:", api.getRequestCount());
}

runDemo();