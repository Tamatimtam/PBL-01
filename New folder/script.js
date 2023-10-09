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

    lightToggle.addEventListener('change', () => {
        toggleStatus('light-status', lightToggle);
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
});


$(document).ready(function() {
    // Click event handler for the LED button
    $('#light-toggle').click(function() {
        $.post('/turn_on_led', function(data) {
            // Handle the response if needed
        });
    });
});

