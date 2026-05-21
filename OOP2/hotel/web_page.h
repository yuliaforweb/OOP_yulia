#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <string>

const std::string GUEST_WEB_PAGE = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Інформаційна система Готель</title>
    <style>
        body { font-family: sans-serif; background: #f0f2f5; padding: 20px; }
        .container { max-width: 600px; background: white; padding: 25px; margin: 0 auto; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .tabs { display: flex; margin-bottom: 20px; border-bottom: 2px solid #ccc; }
        .tab { padding: 10px 20px; cursor: pointer; border: none; background: none; font-size: 16px; width: auto; margin: 0; color: #333; }
        .tab.active { border-bottom: 3px solid #1a73e8; color: #1a73e8; font-weight: bold; }
        .content { display: none; }
        .content.active { display: block; }
        input, select { width: 100%; padding: 10px; margin: 8px 0; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }
        .content button { width: 100%; padding: 10px; margin: 8px 0; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; background: #1a73e8; color: white; border: none; cursor: pointer; font-size: 16px; }
        .content button.cancel-btn { background: #d93025; margin-top: 10px; }
        .error { color: #d93025; margin: 5px 0; font-weight: bold; }
        .success { color: #1e8e3e; margin: 5px 0; font-weight: bold; }
        .booking-card { border: 1px solid #ddd; padding: 15px; margin-top: 15px; border-radius: 6px; background: #fafafa; }

        /* Стилі для блоку вибору оплати */
        .payment-block { margin: 15px 0; padding: 12px; border: 1px dashed #ccc; border-radius: 6px; background: #fdfdfd; }
        .payment-block label { font-weight: bold; display: block; margin-bottom: 8px; }
        .radio-group { display: flex; gap: 20px; align-items: center; margin: 5px 0; }
        .radio-group label { font-weight: normal; display: inline; cursor: pointer; margin: 0; }
        .radio-group input[type="radio"] { width: auto; margin: 0; cursor: pointer; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Інформаційна система "Готель"</h2>
        <div class="tabs">
            <button class="tab active" onclick="switchTab(0)">Вільні номери</button>
            <button class="tab" onclick="switchTab(1)">Резервування</button>
            <button class="tab" onclick="switchTab(2)">Перевірка броні</button>
        </div>

        <div id="tab0" class="content active">
            <label>Дата заїзду:</label>
            <input type="date" id="fund_in">
            <label>Дата виїзду:</label>
            <input type="date" id="fund_out">
            <button onclick="checkFreeRooms()">Знайти вільні номери</button>
            <div id="fund_result"></div>
        </div>

        <div id="tab1" class="content">
            <input type="text" id="b_ln" placeholder="Прізвище">
            <input type="text" id="b_fn" placeholder="Ім'я">
            <input type="text" id="b_mn" placeholder="По батькові">
            <input type="text" id="b_phone" placeholder="Телефон" maxlength="19">
            <input type="text" id="b_pass" placeholder="Паспорт">
            <label>Дата заїзду:</label>
            <input type="date" id="b_in">
            <label>Дата виїзду:</label>
            <input type="date" id="b_out">
            <label>Категорія номеру для проживання:</label>
            <select id="b_cat">
                <option value="1">Стандартний 2-місний (800 грн)</option>
                <option value="2">Стандартний 3-місний (900 грн)</option>
                <option value="3">Делюкс 2-місний з балконом (1500 грн)</option>
                <option value="4">Делюкс 3-місний без балкона (1500 грн)</option>
                <option value="5">Люкс 4-місний (джакузі, 2 кімнати) (3500 грн)</option>
                <option value="6">Люкс 6-місний (джакузі, 3 кімнати) (4200 грн)</option>
            </select>

            <div class="payment-block">
                <label>Оберіть варіант оплати:</label>
                <div class="radio-group">
                    <input type="radio" id="pay_online" name="web_payment" value="online" checked>
                    <label for="pay_online">Оплатити зараз онлайн (Імітація)</label>
                </div>
                <div class="radio-group">
                    <input type="radio" id="pay_reception" name="web_payment" value="upon_check_in">
                    <label for="pay_reception">Оплатити пізніше при заселенні</label>
                </div>
            </div>

            <button onclick="createWebBooking()">Забронювати номер</button>
            <div id="booking_result"></div>
        </div>

        <div id="tab2" class="content">
            <input type="text" id="search_pass" placeholder="Введіть номер паспорта">
            <button onclick="checkGuestStatus()">Перевірити статус замовлення</button>
            <div id="status_result"></div>
        </div>
    </div>

    <script>
        document.getElementById('b_phone').addEventListener('input', function (e) {
            let x = e.target.value.replace(/\D/g, '');
            if (x.startsWith('380')) {
                x = x.substr(3);
            } else if (x.startsWith('0')) {
                x = x.substr(1);
            }
            if (x.length > 9) x = x.substr(0, 9);

            let res = "+38 (0";
            if (x.length > 0) res += x.substr(0, 2);
            if (x.length > 2) res += ") " + x.substr(2, 3);
            if (x.length > 5) res += "-" + x.substr(5, 2);
            if (x.length > 7) res += "-" + x.substr(7);

            if (x.length === 0) res = "";
            e.target.value = res;
        });

        function switchTab(idx) {
            let tabs = document.querySelectorAll('.tab');
            let contents = document.querySelectorAll('.content');
            tabs.forEach((t, i) => t.classList.toggle('active', i === idx));
            contents.forEach((c, i) => c.classList.toggle('active', i === idx));
        }

        function formatDate(isoDate) {
            if (!isoDate) return "";
            let parts = isoDate.split('-');
            if (parts.length !== 3) return isoDate;
            return parts[2] + '.' + parts[1] + '.' + parts[0];
        }

        function checkFreeRooms() {
            let i = document.getElementById('fund_in').value;
            let o = document.getElementById('fund_out').value;
            let res = document.getElementById('fund_result');
            res.innerHTML = "";
            if (!i || !o) {
                res.innerHTML = '<p class="error">Помилка: Заповніть дати заїзду та виїзду!</p>';
                return;
            }
            if (i >= o) {
                res.innerHTML = '<p class="error">Помилка: Дата заїзду не може бути рівною або пізнішою за дату виїзду!</p>';
                return;
            }

            fetch('/api', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ command: 'CHECK_FREE_ROOMS', check_in: formatDate(i), check_out: formatDate(o) })
            })
            .then(r => r.json())
            .then(data => {
                let html = '<h3>Доступні варіанти:</h3>';
                data.rooms.forEach(r => {
                    html += '<p>' + r.desc + ' - ' + r.available_count + ' ном. (' + r.price + ' грн)</p>';
                });
                res.innerHTML = html;
            });
        }

        function createWebBooking() {
            let ln = document.getElementById('b_ln').value;
            let fn = document.getElementById('b_fn').value;
            let mn = document.getElementById('b_mn').value;
            let ph = document.getElementById('b_phone').value;
            let ps = document.getElementById('b_pass').value;
            let i = document.getElementById('b_in').value;
            let o = document.getElementById('b_out').value;
            let cat = document.getElementById('b_cat').value;

            // Зчитуємо обрану радіокнопку платіжного методу
            let payMethod = document.querySelector('input[name="web_payment"]:checked').value;

            let res = document.getElementById('booking_result');
            res.innerHTML = "";

            if (!ln || !fn || !mn || !ph || !ps || !i || !o) {
                res.innerHTML = '<p class="error">Помилка: Усі поля обов\'язкові до заповнення!</p>';
                return;
            }
            if (i >= o) {
                res.innerHTML = '<p class="error">Помилка: Дата заїзду не може бути рівною або пізнішою за дату виїзду!</p>';
                return;
            }

            const roomMap = { "1": 101, "2": 103, "3": 201, "4": 202, "5": 301, "6": 302 };
            let simulatedRoom = roomMap[cat] || 101;

            fetch('/api', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    command: 'CREATE_BOOKING',
                    guest_name: ln + ' ' + fn + ' ' + mn,
                    phone: ph,
                    passport: ps,
                    check_in: formatDate(i),
                    check_out: formatDate(o),
                    room_number: simulatedRoom,
                    payment_method_choice: payMethod // Передаємо вибір у JSON запиті на сервер
                })
            })
            .then(r => r.json())
            .then(data => {
                if (data.status === 'OK') {
                    res.innerHTML = '<p class="success">' + data.message + '</p>';
                } else {
                    res.innerHTML = '<p class="error">' + data.message + '</p>';
                }
            });
        }

        function checkGuestStatus() {
            let ps = document.getElementById('search_pass').value;
            let res = document.getElementById('status_result');
            res.innerHTML = "";
            if (!ps) return;

            fetch('/api', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ command: 'GUEST_CHECK_STATUS', passport: ps })
            })
            .then(r => r.json())
            .then(data => {
                if (!data.bookings || data.bookings.length === 0) {
                    res.innerHTML = '<p class="error">Бронювань для цього паспорта не знайдено</p>';
                    return;
                }
                data.bookings.forEach(b => {
                    let div = document.createElement('div');
                    div.className = 'booking-card';
                    let btnHtml = '';
                    if (b.status_code.trim() === "RESERVED") {
                        btnHtml = '<button class="cancel-btn" onclick="cancelWebBooking(' + b.id + ')">Скасувати бронювання</button>';
                    }

                    // Відображення статусу оплати в картці клієнта на сайті
                    let payStatusText = (b.pay_status === 'paid') ? '<span style="color:#1e8e3e;font-weight:bold;">Оплачено</span>' : '<span style="color:#e67e22;font-weight:bold;">Очікує оплати</span>';

                    div.innerHTML = '<p><b>Замовлення №' + b.id + '</b></p>' +
                                     '<p>Гість: ' + b.guest_name + '</p>' +
                                     '<p>Номер: ' + b.room_number + ' (' + b.room_desc + ')</p>' +
                                     '<p>Період: ' + b.period + '</p>' +
                                     '<p>Статус: ' + b.status_text + '</p>' +
                                     '<p>Оплата: ' + payStatusText + '</p>' +
                                     '<p>Сума: ' + b.total_pay + ' грн</p>' + btnHtml;
                    res.appendChild(div);
                });
            });
        }

        function cancelWebBooking(id) {
            if (navigator.userAgent && confirm("Ви дійсно хочете скасувати це бронювання? Запит потребуватиме підтвердження адміністрацією готелю.")) {
                fetch('/api', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ command: 'CANCEL_BOOKING', id: id })
                })
                .then(r => r.json())
                .then(data => {
                    alert(data.message);
                    checkGuestStatus();
                });
            }
        }
    </script>
</body>
</html>
)html";

#endif