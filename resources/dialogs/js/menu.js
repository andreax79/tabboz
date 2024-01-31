((exports) => {

function addMainMenu(mainMenuEl) {
    let mainMenuElement = mainMenuEl;
    let activated = false;

    if (!mainMenuElement) {
        return;
    }

    // Add event listener for click on menu items
    mainMenuElement.querySelectorAll('li').forEach((item) => {
        item.addEventListener('click', (event) => {
            const classList = event.target.classList;
            if (!classList.contains('disabled') && !classList.contains('menu') && !classList.contains('hotkey')) {
                mainMenuClick(item);
            }
        });
    });

    // Add event listener for click on submenu items
    mainMenuElement.querySelectorAll('li > ul li').forEach((item) =>
        item.addEventListener('click', (event) => {
            if (item.classList.contains('disabled')) {
                event.preventDefault();
            } else {
                closeMenu();
            }
        })
    );

    // Disable click behavior for menu items with the disabled class
    mainMenuElement.querySelectorAll('li.disabled').forEach((item) =>
        item.addEventListener('click', (event) => event.preventDefault())
    );

    // Click on main menu
    function mainMenuClick(el) {
        // Calculate submenu position
        const submenu = el.querySelector('ul');
        const style = getComputedStyle(el);
        const top = parseInt(style.paddingBottom, 10) +
            parseInt(style.paddingTop, 10) +
            el.offsetTop +
            el.offsetHeight -
            2;

        // Set the submenu position
        submenu.style.position = 'absolute';
        submenu.style.top = top + 'px';
        submenu.style.left = el.offsetLeft + 'px';
        submenu.style.zIndex = 100;

        // Toggle the submenu visibility
        const activated = !submenu.hidden;
        if (!activated) {
            el.classList.remove('active-menu');
            submenu.style.display = 'none';
        } else {
            el.classList.add('active-menu');
            submenu.style.display = 'block';
        }

        // Hide other active menus
        if (activated) {
            mainMenuElement.querySelectorAll('.active-menu').forEach((item) => {
                if (item.offsetLeft !== el.offsetLeft) {
                    item.classList.remove('active-menu');
                    item.querySelector('ul').style.display = 'none';
                }
            });
        }
    }

    // Close the menu
    function closeMenu() {
        activated = false;
        mainMenuElement.querySelectorAll('.active-menu').forEach((item) => {
            item.classList.remove('active-menu');
            item.querySelector('ul').style.display = 'none';
        });
    }

    // Bind the 'Esc' key press to close the main menu
    document.addEventListener('keyup', (e) => {
        if (e.keyCode === 27) {
            closeMenu();
        }
    });

    // Bind a click event to the document to close the menu when clicking outside
    document.addEventListener('click', (event) => {
        if (!event.target.classList.contains('active-menu') && !event.target.closest('.active-menu')) {
            closeMenu();
        }
    });
}

function makeDraggable (element) {
    element.addEventListener('mousedown', function(e) {
        if (e.target.classList.contains('title-bar')) {
            var offsetX = e.clientX - parseInt(window.getComputedStyle(this).left);
            var offsetY = e.clientY - parseInt(window.getComputedStyle(this).top);

            function mouseMoveHandler(event) {
                element.style.top = (event.clientY - offsetY) + 'px';
                element.style.left = (event.clientX - offsetX) + 'px';
            }

            function reset() {
                window.removeEventListener('mousemove', mouseMoveHandler);
                window.removeEventListener('mouseup', reset);
            }

            window.addEventListener('mousemove', mouseMoveHandler);
            window.addEventListener('mouseup', reset);
        }
    });
}

exports.addMainMenu = addMainMenu;
exports.makeDraggable = makeDraggable;

})(window);
