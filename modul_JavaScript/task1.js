function summarizeNumbers(numbers) {
    let totalItems = numbers.length;

    // Перевірка на порожній масив
    if (totalItems === 0) {
        return {
            count: 0,
            sum: 0,
            evenCount: 0,
            max: undefined,
            category: "empty"
        };
    }

    let totalSum = 0;
    let evens = 0;
    let highestValue = numbers[0];

    // Обхід масиву через базовий цикл
    for (let currentNum of numbers) {
        totalSum += currentNum;

        // Перевірка на парність
        if (currentNum % 2 === 0) {
            evens++;
        }

        // Пошук максимального значення
        if (currentNum > highestValue) {
            highestValue = currentNum;
        }
    }

    // Визначення категорії суми
    let sumCategory = "non-positive";
    if (totalSum > 0) {
        sumCategory = "positive";
    }

    return {
        count: totalItems,
        sum: totalSum,
        evenCount: evens,
        max: highestValue,
        category: sumCategory
    };
}

// Тести для демонстрації роботи
let roomGuests = [2, 4, 1, 3];
console.log("Статистика заселення номерів:", summarizeNumbers(roomGuests));

let cancelledBookings = [];
console.log("Статистика скасувань:", summarizeNumbers(cancelledBookings));

let receptionBalance = [-5, 2, -10, 4];
console.log("Баланс рецепції:", summarizeNumbers(receptionBalance));