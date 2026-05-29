#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <string>

const std::string GUEST_WEB_PAGE = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>Інформаційна система "Готель"</title>
    <style>
        body { font-family: 'Segoe UI', system-ui, sans-serif; background-color: #f4f6f9; margin: 0; padding: 20px; color: #333; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 12px; box-shadow: 0 4px 15px rgba(0,0,0,0.05); }
        h1 { color: #1a73e8; margin-top: 0; text-align: center; font-size: 28px; }
        .tabs { display: flex; border-bottom: 2px solid #ddd; margin-bottom: 25px; justify-content: center; }
        .tab { padding: 12px 25px; cursor: pointer; font-weight: 600; color: #666; border-bottom: 3px solid transparent; transition: all 0.2s; }
        .tab.active { color: #1a73e8; border-bottom-color: #1a73e8; }
        .content { display: none; }
        .content.active { display: block; }
        label { display: block; margin: 12px 0 6px; font-weight: 500; }
        input, select { width: 100%; padding: 10px; border: 1px solid #ccc; border-radius: 6px; box-sizing: border-box; font-size: 15px; }
        button { width: 100%; padding: 12px; background: #1a73e8; color: white; border: none; border-radius: 6px; font-size: 16px; font-weight: 600; cursor: pointer; margin-top: 18px; transition: background 0.2s; }
        button:hover { background: #155cb4; }
        button:disabled { background: #ccc; cursor: not-allowed; }
        .room-card { border: 1px solid #ddd; padding: 15px; margin-bottom: 15px; border-radius: 8px; display: flex; justify-content: space-between; align-items: center; background: #fbfbfb; }
        .room-info h3 { margin: 0 0 5px 0; color: #222; }
        .room-info p { margin: 0; color: #666; font-size: 14px; }
        .select-btn { width: auto; padding: 8px 20px; margin: 0; background: #28a745; }
        .select-btn:hover { background: #218838; }
        .price-tag { font-weight: bold; color: #28a745; font-size: 18px; margin-top: 10px; text-align: right; }
        .status-box { border: 1px solid #ddd; padding: 15px; margin-top: 15px; border-radius: 8px; background: #f9f9f9; }
        .badge { padding: 4px 8px; border-radius: 4px; font-size: 12px; font-weight: bold; color: white; }
        .badge.active { background: #28a745; }
        .badge.pending { background: #ffc107; color: #222; }
        .badge.cancelled { background: #dc3545; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Інформаційна система "Готель"</h1>
        <div class="tabs">
            <div class="tab active" onclick="switchTab('fund')">Вільні номери</div>
            <div class="tab" onclick="switchTab('booking')">Резервування</div>
            <div class="tab" onclick="switchTab('check')">Перевірка броні</div>
        </div>

        <div id="tab-fund" class="content active">
            <label>Дата заїзду:</label>
            <input type="text" id="fund-ci" placeholder="дд.мм.рррр">
            <label>Дата виїзду:</label>
            <input type="text" id="fund-co" placeholder="дд.мм.рррр">
            <button onclick="searchRooms()">Пошук вільних місць</button>
            <div id="fund-results" style="margin-top: 20px;"></div>
        </div>

        <div id="tab-booking" class="content">
            <label>ПІБ гостя:</label>
            <input type="text" id="book-name" placeholder="Введіть повне ПІБ">
            <label>Телефон:</label>
            <input type="text" id="book-phone" placeholder="+38 (0XX) XXX-XX-XX">
            <label>Паспорт:</label>
            <input type="text" id="book-passport" placeholder="Номер документа (9 цифр або 2 літери + 6 цифр)">
            <label>Дата заїзду:</label>
            <input type="text" id="book-ci" placeholder="дд.мм.рррр" oninput="updateLivePrice()">
            <label>Дата виїзду:</label>
            <input type="text" id="book-co" placeholder="дд.мм.рррр" oninput="updateLivePrice()">
            <label>Категорія номеру для проживання:</label>
            <select id="book-cat" onchange="updateLivePrice()">
                <option value="1" data-price="800">Стандартний 2-місний (800 грн/доба)</option>
                <option value="2" data-price="900">Стандартний 3-місний (900 грн/доба)</option>
                <option value="3" data-price="1500">Делюкс 2-місний з балконом (1500 грн/доба)</option>
                <option value="4" data-price="1500">Делюкс 3-місний без балкона (1500 грн/доба)</option>
                <option value="5" data-price="3500">Люкс 4-місний (джакузі, 2 кімнати) (3500 грн/доба)</option>
                <option value="6" data-price="4200">Люкс 6-місний (джакузі, 3 кімнати) (4200 грн/доба)</option>
            </select>
            <div id="live-price" class="price-tag">Разом до сплати: 0.00 грн</div>
            <button id="submit-booking-btn" onclick="submitBooking()">Надіслати заявку на резерв</button>
            <div id="booking-msg" style="margin-top: 15px; font-weight: bold; text-align: center;"></div>
        </div>

        <div id="tab-check" class="content">
            <label>Введіть номер паспорта:</label>
            <input type="text" id="check-passport" placeholder="Номер документа">
            <button onclick="checkBookingStatus()">Перевірити статус замовлення</button>
            <div id="check-results" style="margin-top: 20px;"></div>
        </div>
    </div>

    <script>
        function switchTab(tabId) {
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.content').forEach(c => c.classList.remove('active'));
            event.target.classList.add('active');
            document.getElementById('tab-' + tabId).classList.add('active');
        }

        // Застосування масок введення
        function setupMasks() {
            const dateInputs = ['fund-ci', 'fund-co', 'book-ci', 'book-co'];
            dateInputs.forEach(id => {
                document.getElementById(id).addEventListener('input', function(e) {
                    let v = e.target.value.replace(/\D/g, '');
                    if (v.length > 8) v = v.substr(0, 8);
                    let res = '';
                    if (v.length > 0) res += v.substr(0, 2);
                    if (v.length > 2) res += '.' + v.substr(2, 2);
                    if (v.length > 4) res += '.' + v.substr(4, 4);
                    e.target.value = res;
                });
            });

            document.getElementById('book-phone').addEventListener('input', function(e) {
                let v = e.target.value.replace(/\D/g, '');
                if (v.startsWith('380')) v = v.substr(3);
                if (v.length > 9) v = v.substr(0, 9);
                let res = '+38 (0' + v.substr(0, 2);
                if (v.length > 2) res += ') ' + v.substr(2, 3);
                if (v.length > 5) res += '-' + v.substr(5, 2);
                if (v.length > 7) res += '-' + v.substr(7, 2);
                e.target.value = res;
            });

            document.getElementById('book-passport').addEventListener('input', function(e) {
                let v = e.target.value.toUpperCase().replace(/[^A-Z0-9]/g, '');
                if (v.length > 9) v = v.substr(0, 9);
                e.target.value = v;
            });
        }

        function calculateDays(ci, co) {
            if (ci.length !== 10 || co.length !== 10) return 0;
            let parts1 = ci.split('.'), parts2 = co.split('.');
            let d1 = new Date(parts1[2], parts1[1]-1, parts1[0]);
            let d2 = new Date(parts2[2], parts2[1]-1, parts2[0]);
            let diff = d2 - d1;
            return diff > 0 ? Math.ceil(diff / (1000 * 60 * 60 * 24)) : 0;
        }

        function updateLivePrice() {
            let ci = document.getElementById('book-ci').value;
            let co = document.getElementById('book-co').value;
            let days = calculateDays(ci, co);
            let select = document.getElementById('book-cat');
            let price = parseFloat(select.options[select.selectedIndex].getAttribute('data-price'));
            let total = days * price;
            document.getElementById('live-price').innerText = "Разом до сплати: " + total.toFixed(2) + " грн (" + days + " діб)";
        }

        function searchRooms() {
            let ci = document.getElementById('fund-ci').value;
            let co = document.getElementById('fund-co').value;
            if (!ci || !co) { alert('Будь ласка, вкажіть дати!'); return; }

            fetch('/api', {
                method: 'POST',
                body: JSON.stringify({ command: 'CHECK_FREE_ROOMS', check_in: ci, check_out: co })
            })
            .then(r => r.json())
            .then(data => {
                let html = '';
                if (data.status === 'OK' && data.rooms) {
                    data.rooms.forEach(r => {
                        let isFull = r.available_count <= 0;
                        html += `<div class="room-card">
                            <div class="room-info">
                                <h3>${r.desc}</h3>
                                <p>Ціна: <b>${r.price} грн/доба</b> | Доступно: ${r.available_count} ном.</p>
                            </div>
                            <button class="select-btn" ${isFull ? 'disabled' : ''} onclick="selectRoomForGuest('${ci}', '${co}', ${r.id})">
                                ${isFull ? 'Зайнято' : 'Обрати'}
                            </button>
                        </div>`;
                    });
                }
                document.getElementById('fund-results').innerHTML = html;
            });
        }

        function selectRoomForGuest(ci, co, catId) {
            document.getElementById('book-ci').value = ci;
            document.getElementById('book-co').value = co;
            document.getElementById('book-cat').value = catId;
            updateLivePrice();
            switchTab('booking');
        }

        function submitBooking() {
            let name = document.getElementById('book-name').value;
            let phone = document.getElementById('book-phone').value;
            let passport = document.getElementById('book-passport').value;
            let ci = document.getElementById('book-ci').value;
            let co = document.getElementById('book-co').value;
            let cat = parseInt(document.getElementById('book-cat').value);

            if (!name || !phone || !passport || !ci || !co) {
                alert('Усі поля форми є обов\'язковими до заповнення!');
                return;
            }

            let roomMap = { 1: 101, 2: 103, 3: 201, 4: 202, 5: 301, 6: 302 };

            fetch('/api', {
                method: 'POST',
                body: JSON.stringify({
                    command: 'CREATE_BOOKING',
                    guest_name: name,
                    phone: phone,
                    passport: passport,
                    check_in: ci,
                    check_out: co,
                    room_number: roomMap[cat],
                    payment_method_choice: 'upon_check_in'
                })
            })
            .then(r => r.json())
            .then(data => {
                let msgBox = document.getElementById('booking-msg');
                if (data.status === 'OK') {
                    msgBox.style.color = '#28a745';
                    msgBox.innerText = data.message;
                } else {
                    msgBox.style.color = '#dc3545';
                    msgBox.innerText = data.message;
                }
            });
        }

        function checkBookingStatus() {
            let pass = document.getElementById('check-passport').value;
            if (!pass) { alert('Введіть номер паспорта!'); return; }

            fetch('/api', {
                method: 'POST',
                body: JSON.stringify({ command: 'GUEST_CHECK_STATUS', passport: pass })
            })
            .then(r => r.json())
            .then(data => {
                let html = '';
                if (data.status === 'OK' && data.bookings && data.bookings.length > 0) {
                    data.bookings.forEach(b => {
                        let stClass = b.status_text.includes('Скасовано') ? 'cancelled' : 'active';
                        html += `<div class="status-box">
                            <p><b>Ордер №:</b> ${b.id} | <b>Номер фонду:</b> №${b.room}</p>
                            <p><b>Період проживання:</b> ${b.period}</p>
                            <p><b>Сума до сплати:</b> ${b.total_pay} грн</p>
                            <p><b>Статус ордера:</b> <span class="badge ${stClass}">${b.status_text}</span></p>
                            ${stClass !== 'cancelled' ? `<button style="background:#dc3545; padding:6px; font-size:14px; margin-top:8px;" onclick="cancelWebBooking(${b.id})">Скасувати бронь</button>` : ''}
                        </div>`;
                    });
                } else {
                    html = '<p style="color:#dc3545; text-align:center;">Активних бронювань за цим документом не знайдено.</p>';
                }
                document.getElementById('check-results').innerHTML = html;
            });
        }

        function cancelWebBooking(id) {
            if (!confirm('Ви дійсно бажаєте скасувати це бронювання?')) return;
            fetch('/api', {
                method: 'POST',
                body: JSON.stringify({ command: 'CANCEL_BOOKING', id: id })
            })
            .then(r => r.json())
            .then(data => {
                alert(data.message);
                checkBookingStatus();
            });
        }

        window.onload = function() {
            setupMasks();
        };
    </script>
</body>
</html>
)html";

#endif // WEB_PAGE_H