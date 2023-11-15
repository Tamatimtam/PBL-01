document.addEventListener('DOMContentLoaded', function () {
    const lightbulbButton = document.getElementById('lightbulb-button');
    const acButton = document.getElementById('ac-button');
    const lightbulbMenu = document.getElementById('lightbulb-menu');
    const acMenu = document.getElementById('ac-menu');
    const lightToggle = document.getElementById('light-toggle');
    const acToggle = document.getElementById('ac-toggle');

    lightbulbButton.addEventListener('click', () => {
        toggleMenu(lightbulbMenu);
    });

    acButton.addEventListener('click', () => {
        toggleMenu(acMenu);
    });

    // Combine the toggleLight and updateLEDStatus functions into a single event listener
    lightToggle.addEventListener('change', () => {
        toggleStatus('light-status', lightToggle);
        toggleLight(); // Call the toggleLight function when the toggle changes
    });

    acToggle.addEventListener('change', () => {
        toggleStatus('ac-status', acToggle);
    });

    function toggleMenu(menu) {
        if (menu.classList.contains('expanded')) {
            menu.classList.remove('expanded');
            setTimeout(() => {
                menu.style.display = 'none';
            }, 300); // Delay for the transition to complete
        } else {
            menu.style.display = 'block';
            setTimeout(() => {
                menu.classList.add('expanded');
            }, 10); // Delay to trigger animation
        }
    }

    function toggleStatus(elementId, toggle) {
        const statusElement = document.getElementById(elementId);
        if (toggle.checked) {
            statusElement.textContent = 'On';
        } else {
            statusElement.textContent = 'Off';
        }
    }

    function toggleLight() { //define toggle light
        fetch('/turn_on_led') //fetch the turn on led route
            .then(response => response.text())
            .then(data => {
                // Handle the response (if needed)
                console.log('Light toggle response:', data);
            })
            .catch(error => {
                console.error('Error:', error);
            });
    }

    function updateLEDStatus() {
        fetch('/get_led_status') //gets response from server
            .then(response => response.text()) //turns the response into text
            .then(data => { //takes int the response and processes it to the next few lines
                const lightStatusElement = document.getElementById('light-status');
                const lightToggle = document.getElementById('light-toggle');
                lightStatusElement.textContent = data;
                lightToggle.checked = data === 'ON'; // Set the checkbox status
            })
            .catch(error => {
                console.error('Error:', error);
            });
    }

    // Initial update when the page loads
    updateLEDStatus();
});
