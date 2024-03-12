<div align="center">
  <a href="https://andreax79.github.io/tabboz/" target="_blank"><img alt="Tabboz Simulator" src="https://github.com/andreax79/tabboz/assets/1288154/64269165-8958-4f3a-945a-82c3ffffd4de"/></a>
</div>
<br/>

Simply tap the icon to launch the Tabboz Simulator directly in your browser.

## 🖥️ Web development

<div align="center">
  <a href="https://andreax79.github.io/tabboz/" target="_blank"><img alt="Tabboz Simulator Screenshot" src="https://github.com/andreax79/tabboz/assets/1288154/0999ef77-3e7a-45a0-9a0b-1206a6f5bddd"/></a>
</div>
<br/>

### 📖 Prerequisites

In order to compile the project we need the following software installed on our development machine:
- Python >= 3.8
- [Emscripten](https://emscripten.org/)

### 📦 Builds

Build the project.

```shell
make build
```

We also have a script for updating dialogs used in the project, you only need to run this if you modify the `.rc` files:

```shell
make rc
```

### 🎨 Code linting

To fix the linting errors, use the following command:

```shell
make format
```

### 🚀 Links

- [Novantotto, A tool for porting Windows programs to WebAssembly](https://github.com/andreax79/novantotto)
- [Emscripten, A complete compiler toolchain to WebAssembly](https://emscripten.org/)
- [98.css, A design system for building faithful recreations of old UIs](https://github.com/jdan/98.css)

## ⚖️ LICENSE

[![GPL 3](https://www.gnu.org/graphics/gplv3-or-later.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)
