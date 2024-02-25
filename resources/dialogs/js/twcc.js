/* Tabboz Windows Custom Controls */
((exports) => {

    _resolve = null;
    _activeWindowHwnd = null;

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
            if (e.target.classList.contains('title-bar') || e.target.classList.contains('title-bar-text')) {
                const offsetX = e.clientX - parseInt(window.getComputedStyle(this).left);
                const offsetY = e.clientY - parseInt(window.getComputedStyle(this).top);

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

    // Stop waiting for an event
    function stopWaiting() {
        if (_resolve) {
            _resolve();
            _resolve = null;
        }
    }

    // Wait for an event
    function waitEvent() {
        return new Promise((resolve, reject) => {
            stopWaiting();
            _resolve = resolve;
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
    function setActiveWindow(hWnd) {
        _activeWindowHwnd = hWnd;
        // Add 'inactive' class to other windows
        document.querySelectorAll(".title-bar").forEach(element => {
            if (!element.classList.contains("inactive")) {
                element.classList.add("inactive");
            }
        });
        // Remove 'inactive' class from the active window
        const element = document.querySelector(`#win${hWnd} .title-bar`);
        if (element != null) {
            element.classList.remove("inactive");
        }
    };

    // Show/hide a window
    function showWindow(hWnd, show) {
        const win = document.querySelector('#win' + hWnd);
        if (win != null) {
            win.style.display = show ? 'block' : 'none';
            return true;
        } else {
            return false;
        }
    };

    // Show/hide all the windows
    function showApp(show) {
        document.querySelectorAll('.window,.wall').forEach((item) => item.style.display = show ? 'block' : 'none');
    };

    // Associate a icon with a window
    function setIcon(hWnd, icon) {
        const element = document.querySelector(`#win${hWnd} .title-bar-text`);
        if (element != null) {
            element.innerHTML = `<img src="resources/icons/${icon}.gif" height="16" />` + element.innerText
        }
    };

    // Change the position and dimensions of the specified window
    function moveWindow(hWnd, X, Y, nWidth, nHeight) {
        const win = document.querySelector(`#win${hWnd}`);
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
    async function drawImage(hWnd, lpCanvasClass, imageId, x, y) {
        const canvasClass = UTF8ToString(lpCanvasClass);
        const canvas = document.querySelector(`#win${hWnd} .${canvasClass}`);
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
    function getWindowRectDimension(hWnd, dimension) {
        const win = document.querySelector(`#win${hWnd}`);
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
    function setDlgItemText(hWnd, nIDDlgItem, lpString) {
        let control = document.querySelector(`#win${hWnd} .control${nIDDlgItem}`);
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
    function getDlgItemText(hWnd, nIDDlgItem, lpString, nMaxCount) {
        let control = document.querySelector(`#win${hWnd} input.control${nIDDlgItem}`);
        if (control != null) {
            stringToUTF8(control.value, lpString, nMaxCount);
            return control.value.length;
        }
        control = document.querySelector(`#win${hWnd} .control${nIDDlgItem}`);
        if (control != null) {
            stringToUTF8(control.innerText, lpString, nMaxCount);
            return control.innerText.length;
        } else {
            return 0;
        }
    }

    // Set the check state of a radio button or check box
    function setCheck(hWnd, nIDDlgItem, wParam) {
        const control = document.querySelector(`#win${hWnd} .control${nIDDlgItem}`);
        if (control != null) {
            control.checked = (wParam != 0);
        }
        return 0;
    }

    // Get the check state of a radio button or check box
    function getCheck(hWnd, nIDDlgItem) {
        const control = document.querySelector(`#win${hWnd} .control${nIDDlgItem}`);
        if (control != null) {
            return control.checked;
        }
        return 0;
    }

    // Add a string to the list box of a combo box
    function comboBoxAddString(hWnd, nIDDlgItem, lpString) {
        const control = document.querySelector(`#win${hWnd} select.control${nIDDlgItem}`);
        if (control != null) {
            const option = document.createElement('option');
            option.text = UTF8ToString(lpString);
            control.add(option, 0);
        }
        return 0;
    }

    // Select a string in the list of a combo box
    function comboBoxSelect(hWnd, nIDDlgItem, wParam) {
        const control = document.querySelector(`#win${hWnd} select.control${nIDDlgItem}`);
        if (control != null) {
            control.selectedIndex = wParam;
        }
        return 0;
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

    // Show a message box
    async function messageBox(hWnd, lpText, lpCaption, uType, parentWindowId) {
        const element = document.getElementById('messagebox');
        const wall = document.getElementById('wall').cloneNode(true);
        const c = element.cloneNode(true);
        // Set window id
        wall.id = 'wall' + hWnd;
        c.id = 'win' + hWnd;
        // Icon
        if (uType & 0x00000020) { // MB_ICONQUESTION
            c.querySelector('img').src = "resources/icons/102.png";
        } else if (uType & 0x00000010) { // MB_ICONSTOP
            c.querySelector('img').src = "resources/icons/103.png";
        } else if (uType & 0x00000030) { // MB_ICONEXCLAMATION
            c.querySelector('img').src = "resources/icons/101.png";
        } else if (uType & 0x00000040) { // MB_ICONINFORMATION
            c.querySelector('img').src = "resources/icons/104.png";
        }
        // Buttons
        if (uType & 0x00000001) { // MB_OKCANCEL
            c.querySelector('.control1').style.display = 'inline';
            c.querySelector('.control2').style.display = 'inline';
            c.querySelector('.control6').style.display = 'none';
            c.querySelector('.control7').style.display = 'none';
            c.querySelector('.control1').focus({
                focusVisible: true
            });
        } else if (uType & 0x00000004) { // MB_YESNO
            c.querySelector('.control1').style.display = 'none';
            c.querySelector('.control2').style.display = 'none';
            c.querySelector('.control6').style.display = 'inline';
            c.querySelector('.control7').style.display = 'inline';
            c.querySelector('.control6').focus({
                focusVisible: true
            });
        } else { // MB_OK
            c.querySelector('.control1').innerText = 'OK';
            c.querySelector('.control1').style.display = 'inline';
            c.querySelector('.control2').style.display = 'none';
            c.querySelector('.control6').style.display = 'none';
            c.querySelector('.control7').style.display = 'none';
            c.querySelector('.control1').focus({
                focusVisible: true
            });
        }
        c.style.position = 'absolute';
        // Set title and message
        c.querySelector('.title-bar-text').innerText = UTF8ToString(lpCaption);
        c.querySelector('.content').innerText = UTF8ToString(lpText);
        // Add window to screen
        const destination = document.getElementById('screen');
        destination.appendChild(wall);
        destination.appendChild(c);
        // Set focus
        if (uType & 0x00000001) { // MB_OKCANCEL
            c.querySelector('.control1').focus({
                focusVisible: true
            });
        } else if (uType & 0x00000004) { // MB_YESNO
            c.querySelector('.control6').focus({
                focusVisible: true
            });
        } else { // MB_OK
            c.querySelector('.control1').focus({
                focusVisible: true
            });
        }
        // Center the dialog
        centerDialog(c);
        // Activate the window
        setActiveWindow(hWnd);
        // Make the window draggrable
        makeDraggable(c);
    };

    // Center the dialog
    function centerDialog(element) {
        let x = parseInt(getComputedStyle(document.getElementById('screen')).width);
        let y = parseInt(getComputedStyle(document.getElementById('screen')).height);
        let w = parseInt(getComputedStyle(element).width);
        let h = parseInt(getComputedStyle(element).height);
        x = (x - w) / 2;
        y = (y - h) / 2;
        element.style.left = x + 'px';
        element.style.top = y + 'px';
    }

    // Show dialog box
    async function dialogBox(hWnd, dialog, parentWindowId) {
        // Load html
        const response = await fetch("resources/dialogs/includes/" + dialog + ".inc.html");
        const html = await response.text();
        const wall = document.getElementById('wall').cloneNode(true);
        const c = createElementFromHTML(html);
        // Set window id
        wall.id = 'wall' + hWnd;
        c.id = 'win' + hWnd;
        // Set window position
        setWindowInitialPosition(c, parentWindowId);
        // Add window to screen
        const destination = document.getElementById('screen');
        destination.appendChild(wall);
        destination.appendChild(c);
        // Activate the window
        setActiveWindow(hWnd);
        // Add menu
        addMainMenu(c);
        // Make the window draggrable
        makeDraggable(c);
        // Add DlgItems
        c.querySelectorAll('.dlg_item').forEach(element => {
            const hMenu = Number(element.className.match(/\d+/));
            _AllocateDlgItem(hWnd, hMenu);
        });
    }

    // Destroy a dialog box
    function destroyDialogBox(hWnd) {
        const destination = document.getElementById('screen');
        destination.removeChild(document.getElementById('wall' + hWnd));
        destination.removeChild(document.getElementById('win' + hWnd));
    }

    // Set window position
    function setWindowInitialPosition(c, parentWindowId) {
        if (parentWindowId >= 0) {
            const parent = document.getElementById('win' + parentWindowId);
            if (parent != null) {
                const style = getComputedStyle(parent);
                c.style.left = (parseInt(style.left) + 40) + 'px';
                c.style.top = (parseInt(style.top) + 40) + 'px';
            }
        }
    }

    // Load a string resource into a buffer
    function loadString(uID, lpBuffer, cchBufferMax) {
        const value = window.strings[uID] || "";
        stringToUTF8(value, lpBuffer, cchBufferMax);
        return value.length;
    };

    // Load strings
    async function loadStringResources() {
        const response = await fetch("resources/strings/strings.json");
        window.strings = await response.json();
    };

    // Preload
    async function preload() {
        const response = await fetch("resources/bitmaps/list.json");
        (await response.json()).data.forEach(element => {
            (new Image()).src = `resources/bitmaps/${element}`;
        });
    }

    // Calculate the position of an event
    function calculateClickPosition(event) {
        // Get click position
        const rect = event.target.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        // low-order word specifies the x-coordinate, high-order word specifies the y-coordinate
        return ((y & 0xffff) << 16) + (x & 0xffff);
    }

    // Return true if element is a checkbox
    function isCheckbox(element) {
        return element.nodeName == "INPUT" && element.type == "checkbox";
    }

    // Add event listener
    function eventListenerSetup() {
        document.querySelector('body').addEventListener('click', eventHandler);
        document.querySelector('body').addEventListener('keydown', eventHandler);
        document.querySelector('body').addEventListener('input', eventHandler);
    }

    function eventHandler(event) {
        console.log(event);
        const match = event.target.className.match(/\d+/);
        switch (event.type) {
            case 'click':
                if (match) {
                    const message = 0x0111; // WM_COMMAND
                    const wParam = Number(match[0]);
                    const lParam = calculateClickPosition(event);
                    _PostMessage(_activeWindowHwnd, message, wParam, lParam);
                }
                break;
            case 'input':
                if (match && !isCheckbox(event.target)) {
                    const message = 0x0111; // WM_COMMAND
                    const wParam = Number(match[0]);
                    const lParam = 0;
                    _PostMessage(_activeWindowHwnd, message, wParam, lParam);
                }
                break;
            case 'keydown':
                if (event.keyCode == 27) { // ESC
                    const message = 0x100; // WM_KEYDOWN
                    const wParam = 0x1B; // VK_ESCAPE
                    const lParam = 0;
                    _PostMessage(_activeWindowHwnd, message, wParam, lParam);
                } else if (event.target.nodeName == "BUTTON" && event.keyCode == 13 && match) { // Enter
                    const message = 0x0111; // WM_COMMAND
                    const wParam = Number(match[0]);
                    const lParam = 0;
                    _PostMessage(_activeWindowHwnd, message, wParam, lParam);
                }
                break;
        }
        stopWaiting();
    }

    // Display shutdown screen
    function shutdown() {
        document.querySelector("#shutdown").style.display = "block";
    }

    exports.addMainMenu = addMainMenu;
    exports.makeDraggable = makeDraggable;
    exports.waitEvent = waitEvent;
    exports.stopWaiting = stopWaiting;
    exports.createElementFromHTML = createElementFromHTML;
    exports.setActiveWindow = setActiveWindow;
    exports.showWindow = showWindow;
    exports.showApp = showApp;
    exports.setIcon = setIcon;
    exports.moveWindow = moveWindow;
    exports.getWindowRectDimension = getWindowRectDimension;
    exports.drawImage = drawImage;
    exports.setDlgItemText = setDlgItemText;
    exports.getDlgItemText = getDlgItemText;
    exports.setCheck = setCheck;
    exports.comboBoxAddString = comboBoxAddString;
    exports.comboBoxSelect = comboBoxSelect;
    exports.getSystemMetrics = getSystemMetrics;
    exports.loadString = loadString;
    exports.loadStringResources = loadStringResources;
    exports.messageBox = messageBox;
    exports.dialogBox = dialogBox;
    exports.destroyDialogBox = destroyDialogBox;
    exports.preload = preload;
    exports.eventListenerSetup = eventListenerSetup;
    exports.shutdown = shutdown;

})(window);