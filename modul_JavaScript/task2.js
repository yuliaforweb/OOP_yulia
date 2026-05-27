function processProducts(products) {
    // Відбір назв доступних послуг через filter та map
    let available = products
        .filter(item => item.inStock === true)
        .map(item => item.name);

    // Підрахунок суми цін через reduce
    let totalPrice = products
        .filter(item => item.inStock === true)
        .reduce((sum, item) => sum + item.price, 0);

    // Пошук найдешевшої послуги серед наявних
    let availableItems = products.filter(item => item.inStock === true);

    let cheapest = undefined;
    if (availableItems.length > 0) {
        let cheapestItem = availableItems[0];

        for (let i = 1; i < availableItems.length; i++) {
            if (availableItems[i].price < cheapestItem.price) {
                cheapestItem = availableItems[i];
            }
        }
        cheapest = cheapestItem.name;
    }

    // Створення повного прайс-листа через map
    let priceList = products.map(item => `${item.name} ${item.price} грн`);

    return {
        available: available,
        totalPrice: totalPrice,
        cheapest: cheapest,
        priceList: priceList
    };
}

// Дані для перевірки програми
const hotelServices = [
    { name: "Сніданок (шведський стіл)", price: 250, inStock: true },
    { name: "Трансфер з вокзалу", price: 600, inStock: false },
    { name: "Відвідування майстер-класу з арт-мистецтва", price: 2000, inStock: true },
    { name: "Міні-бар у номері", price: 150, inStock: true }
];

console.log("--- Аналіз послуг готелю ---");
console.log(processProducts(hotelServices));