/* Tabboz Windows Custom Controls */
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

    // Make a window draggrable
    function makeDraggable(element) {
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

    async function waitListener(windowId) {
        // return await waitListenerS(`#win${windowId}, #win${windowId} input`);
        return await waitListenerS(`#win${windowId} button, #win${windowId} input, #win${windowId} .menu, #win${windowId} img, #win${windowId} canvas`);
    }

    function waitListenerS(selector) {
        return new Promise(function(resolve, reject) {
            var listener = event => {
                document.querySelectorAll(selector).forEach(element => {
                    const listenerName = (element.nodeName == "INPUT") ? "input" : "click";
                    element.removeEventListener(listenerName, listener)
                });
                const match = event.target.className.match(/\d+/);
                const controlId = match ? Number(match[0]) : 0;
                let rect = event.target.getBoundingClientRect();
                let x = event.clientX - rect.left;
                let y = event.clientY - rect.top;
                resolve({
                    controlId: controlId,
                    x: x,
                    y: y
                });
            };
            document.querySelectorAll(selector).forEach(element => {
                const listenerName = (element.nodeName == "INPUT") ? "input" : "click";
                element.addEventListener(listenerName, listener)
            });
        });
    }

    // Create elements form HTML
    function createElementFromHTML(html) {
        const template = document.createElement('template');
        template.innerHTML = html.trim();
        const result = template.content.children;
        if (result.length === 1) {
            return result[0];
        } else {
            return result;
        }
    }

    // Change the active window
    function setActiveWindow(windowId) {
        document.querySelectorAll(".title-bar").forEach(element => {
            if (!element.classList.contains("inactive")) {
                element.classList.add("inactive");
            }
        });
        const element = document.querySelector(`#win${windowId} .title-bar`);
        if (element != null) {
            element.classList.remove("inactive");
        }
    };

    // Show/hide a window
    function showWindow(windowId, show) {
        const win = document.querySelector('#win' + windowId);
        if (win != null) {
            win.style.display = show ? 'block' : 'none';
            return true;
        } else {
            return false;
        }
    };

    // Change the position and dimensions of the specified window
    function moveWindow(windowId, X, Y, nWidth, nHeight) {
        const win = document.querySelector('#win' + windowId);
        if (win == null) {
            return false;
        } else {
            win.style.left = X + 'px';
            win.style.top = Y + 'px';
            win.style.width = nWidth + 'px';
            win.style.height = nHeight + 'px';
            return true;
        }
    };

    // Draw an image on a canvas
    async function drawImage(windowId, canvasClass, imageId, x, y) {
        const canvas = document.querySelector(`#win${windowId} .${canvasClass}`);
        if (canvas) {
            // Load image
            const url = `resources/bitmaps/${imageId}.png`;
            const image = new Image();
            await new Promise(r => image.onload = r, image.src = url);
            // Draw image
            canvas.getContext("2d").drawImage(image, x, y);
        }
    }

    // Retrieve the dimensions of a specified window
    function getWindowRectDimension(windowId, dimension) {
        const win = document.querySelector('#win' + windowId);
        if (win == null) {
            return 0;
        }
        const style = getComputedStyle(win);
        switch (dimension) {
            case 0: // left
                return parseInt(style.left);
            case 1: // top
                return parseInt(style.top);
            case 2: // right
                return parseInt(style.left) + parseInt(style.width);
            case 3: // bottom
                return parseInt(style.top) + parseInt(style.height);
            default:
                return 0;
        }
    }

    // Set the title or text of a control in a dialog box
    function setDlgItemText(windowId, nIDDlgItem, lpString) {
        let control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
        if (control == null) {
            return false;
        } else if (control.tagName == "INPUT") {
            if (control.type == "radio") {
                // Set radio label
                const label = control.parentNode.querySelector("label");
                if (label != null) {
                    label.innerText = UTF8ToString(lpString);
                }
            } else {
                // Set text
                control.value = UTF8ToString(lpString);
            }
        } else {
            control.innerText = UTF8ToString(lpString);
        }
        return true;
    }

    // Retrieve the title or text associated with a control in a dialog box
    function getDlgItemText(windowId, nIDDlgItem, lpString, nMaxCount) {
        let control = document.querySelector('#win' + windowId + ' input.control' + nIDDlgItem);
        if (control != null) {
            stringToUTF8(control.value, lpString, nMaxCount);
            return control.value.length;
        }
        control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
        if (control != null) {
            stringToUTF8(control.innerText, lpString, nMaxCount);
            return control.innerText.length;
        } else {
            return 0;
        }
    }

    // Retrieve the specified system metric
    function getSystemMetrics(nIndex) {
        switch (nIndex) {
            case 0: // SM_CXSCREEN
                return parseInt(getComputedStyle(document.getElementById('screen')).width);
            case 1: // SM_CYSCREEN
                return parseInt(getComputedStyle(document.getElementById('screen')).height);
            default:
                return 0;
        }
        return 0;
    }

    // Load strings
    async function loadStringResources() {
        const response = await fetch("resources/strings/strings.json");
        window.strings = await response.json();
    };

    async function preload() {
        const response = await fetch("resources/bitmaps/list.json");
        (await response.json()).data.forEach(element => {
            (new Image()).src = `resources/bitmaps/${element}`;
        });
    }

    // Display shutdown screen
    function shutdown() {
        document.querySelector("#shutdown").style.display = "block";
    }

    exports.addMainMenu = addMainMenu;
    exports.makeDraggable = makeDraggable;
    exports.waitListener = waitListener;
    exports.createElementFromHTML = createElementFromHTML;
    exports.setActiveWindow = setActiveWindow;
    exports.showWindow = showWindow;
    exports.moveWindow = moveWindow;
    exports.getWindowRectDimension = getWindowRectDimension;
    exports.drawImage = drawImage;
    exports.setDlgItemText = setDlgItemText;
    exports.getDlgItemText = getDlgItemText;
    exports.getSystemMetrics = getSystemMetrics;
    exports.loadStringResources = loadStringResources;
    exports.preload = preload;
    exports.shutdown = shutdown;

})(window);