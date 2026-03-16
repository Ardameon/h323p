# Этап 7: Документация и релиз

**Длительность:** 1 неделя  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этапы 1-6 (полная функциональность)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Структура документации](#-структура-документации)
4. [Детальная реализация](#-детальная-реализация)
5. [CI/CD настройка](#-ci/cd-настройка)
6. [Критерии завершения](#-критерии-завершения)
7. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 7 — подготовить проект к публикации: создать полную документацию, настроить CI/CD, подготовить примеры и выпустить версию v0.1.0.

### Результаты этапа

```
✅ README.md с быстрым стартом
✅ Полное CLI руководство
✅ Примеры сценариев (10+)
✅ CI/CD пайплайн (GitHub Actions)
✅ Сборка для Linux
✅ Документация API
✅ Релиз v0.1.0 на GitHub
✅ Changelog ведётся
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 7.1 | README.md | 2 дня | ⏳ |
| 7.2 | CLI справочник | 2 дня | ⏳ |
| 7.3 | Примеры сценариев | 1 день | ⏳ |
| 7.4 | CI/CD | 2 дня | ⏳ |
| 7.5 | Релиз v0.1.0 | 1 день | ⏳ |
| **Итого** | | **8 дней** | |

---

## 📁 Структура документации

```
h323p/
├── README.md                     # Главный README
├── INSTALL.md                    # Инструкция по установке
├── CHANGELOG.md                  # История изменений
├── LICENSE                       # Лицензия (MPL)
├── docs/
│   ├── getting_started.md        # Быстрый старт
│   ├── cli_reference.md          # CLI справочник
│   ├── xml_scenarios.md          # XML сценарии руководство
│   ├── architecture.md           # Архитектура
│   ├── development.md            # Разработка
│   ├── testing.md                # Тестирование
│   └── faq.md                    # FAQ
├── examples/
│   ├── scenarios/
│   │   ├── basic_call.xml        # Базовый вызов
│   │   ├── gatekeeper_call.xml   # Вызов через GK
│   │   ├── load_test.xml         # Нагрузочный тест
│   │   ├── fuzzing.xml           # Fuzzing тест
│   │   ├── video_call.xml        # Видео вызов
│   │   ├── h239_presentation.xml # H.239 презентация
│   │   └── tls_call.xml          # TLS вызов
│   ├── csv/
│   │   └── destinations.csv      # Пример CSV
│   └── scripts/
│       ├── run_test.sh           # Скрипт запуска
│       └── analyze_results.py    # Анализ результатов
└── .github/
    └── workflows/
        ├── build.yml             # Сборка
        ├── test.yml              # Тесты
        └── release.yml           # Релиз
```

---

## 🔧 Детальная реализация

### 7.1 README.md

**Файл: `README.md`**

```markdown
# H.323 Test Utility (h323p)

[![Build Status](https://github.com/h323p/h323p/workflows/Build/badge.svg)](https://github.com/h323p/h323p/actions)
[![License: MPL](https://img.shields.io/badge/License-MPL-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)
[![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)](https://github.com/h323p/h323p/releases)

**h323p** — утилита для тестирования H.323 вызовов по аналогии с SIPP для SIP.

## ✨ Возможности

- 📞 **Нагрузочное тестирование** — до 1000+ одновременных вызовов
- 🎭 **Fuzzing тестирование** — RTP, H.225, H.245
- 📹 **Видео поддержка** — H.261, H.263, H.239 презентации
- 🔒 **TLS шифрование** — защищённая сигнализация
- 📊 **Статистика и отчёты** — JSON, CSV, HTML
- 📜 **XML сценарии** — гибкое описание тестов

## 🚀 Быстрый старт

### Установка зависимостей

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libssl-dev libpugixml-dev

# H323Plus (требуется сборка из исходников)
git clone https://github.com/willamowius/h323plus.git
cd h323plus
./configure --enable-h235
make debugnoshared
export OPENH323DIR=$(pwd)
export PTLIBDIR=$(pwd)/../ptlib
```

### Сборка

```bash
git clone https://github.com/h323p/h323p.git
cd h323p
mkdir build && cd build
cmake .. -DH323PLUS_DIR=$OPENH323DIR
make -j$(nproc)
```

### Первый запуск

```bash
# Базовый вызов
./h323p call 192.168.1.100

# Нагрузочный тест
./h323p load 192.168.1.100 -c 100 -r 10

# Прослушивание
./h323p listen
```

## 📚 Документация

- [Установка](INSTALL.md) — подробная инструкция
- [CLI справочник](docs/cli_reference.md) — все команды и опции
- [XML сценарии](docs/xml_scenarios.md) — создание сценариев
- [Архитектура](docs/architecture.md) — внутреннее устройство

## 📋 Примеры использования

### Нагрузочный тест с отчётом

```bash
./h323p load 192.168.1.100 \
  --calls 500 \
  --rate 50 \
  --report report.json \
  --cdr calls.csv
```

### Fuzzing тест

```bash
./h323p fuzz 192.168.1.100 \
  --type rtp \
  --percentage 50 \
  --max-packets 10000
```

### Видео вызов с презентацией

```bash
./h323p call 192.168.1.100 \
  --video \
  --video-codec H.263 \
  --h239 \
  --h239-role presenter
```

## 🧪 Тестирование

```bash
# Запуск тестов
ctest --output-on-failure

# Тесты с покрытием
cmake .. -DCOVERAGE=ON
make coverage
```

## 📊 Поддерживаемые протоколы

| Протокол | Статус | Описание |
|----------|--------|----------|
| H.225 RAS | ✅ | Регистрация на Gatekeeper |
| H.225 Q.931 | ✅ | Сигнализация вызовов |
| H.245 | ✅ | Управление каналами |
| RTP/RTCP | ✅ | Передача медиа |
| H.235 | ✅ | Шифрование |
| H.239 | ✅ | Презентации |
| H.460.x | ⏳ | NAT traversal |

## 📦 Зависимости

| Зависимость | Версия | Обязательная |
|-------------|--------|--------------|
| H323Plus | 1.28.0+ | ✅ |
| PTLib | 2.12.0+ | ✅ |
| OpenSSL | 1.1.1+ | ✅ |
| pugixml | 1.11+ | ✅ |
| CLI11 | 2.3+ | ❌ |
| spdlog | 1.10+ | ❌ |

## 🔧 Конфигурация сборки

```bash
cmake .. \
  -DH323PLUS_DIR=/path/to/h323plus \
  -DENABLE_VIDEO=ON \
  -DENABLE_TLS=ON \
  -DENABLE_FUZZING=ON \
  -DBUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Release
```

## 📝 Лицензия

Распространяется под лицензией [MPL-2.0](LICENSE).

## 👥 Авторы

- [Ваше имя] — основная разработка
- [Список контрибьюторов](https://github.com/h323p/h323p/graphs/contributors)

## 🤝 Contributing

1. Fork репозиторий
2. Создайте feature branch (`git checkout -b feature/amazing-feature`)
3. Commit изменений (`git commit -m 'Add amazing feature'`)
4. Push в branch (`git push origin feature/amazing-feature`)
5. Откройте Pull Request

## 📞 Поддержка

- Issues: https://github.com/h323p/h323p/issues
- Discussions: https://github.com/h323p/h323p/discussions
- Email: support@h323p.org

## 🙏 Благодарности

- [H323Plus](https://www.h323plus.org/) — H.323 стек
- [callgen323](https://github.com/willamowius/callgen323) — референсная реализация
- [SIPP](https://github.com/SIPp/sipp) — вдохновение для CLI
```

---

### 7.2 CLI справочник

**Файл: `docs/cli_reference.md`**

```markdown
# CLI справочник

## Обзор

h323p использует подкоманды для различных режимов работы:

```bash
h323p <command> [global options] [command options] [arguments]
```

## Команды

### call

Запуск H.323 вызова.

```bash
h323p call <destination> [options]
```

**Аргументы:**

| Аргумент | Описание |
|----------|----------|
| `destination` | Адрес назначения (IP, alias, URI) |

**Опции:**

| Опция | Короткая | Описание | По умолчанию |
|-------|----------|----------|--------------|
| `--scenario` | `-s` | XML сценарий | — |
| `--gatekeeper` | `-g` | Gatekeeper адрес | auto-discover |
| `--username` | `-u` | Имя пользователя | — |
| `--password` | `-p` | Пароль для GK | — |
| `--video` | `-V` | Включить видео | false |
| `--audio-codec` | `-P` | Аудиокодек | G.711 |
| `--video-codec` | — | Видеокодек | H.264 |
| `--h239` | — | H.239 презентации | false |
| `--media-enc` | — | Шифрование (128/192/256) | off |
| `--tls` | — | TLS | false |
| `--max-calls` | `-m` | Макс. вызовов | 10 |
| `--repeat` | `-r` | Повторов | 1 |
| `--cycle` | `-C` | Циклический режим | false |
| `--tmin-call` | — | Мин. длительность (сек) | 10 |
| `--tmax-call` | — | Макс. длительность (сек) | 30 |
| `--cdr` | — | CDR файл | — |
| `--trace` | `-t` | Уровень трассировки | 0 |
| `--help` | `-h` | Помощь | — |

**Примеры:**

```bash
# Простой вызов
h323p call 192.168.1.100

# Через Gatekeeper
h323p call alias:1001 -g 192.168.1.1 -u test -p secret

# С видео и H.239
h323p call 192.168.1.100 -V --h239

# По сценарию
h323p call --scenario scenarios/custom.xml
```

---

### listen

Режим прослушивания входящих вызовов.

```bash
h323p listen [options]
```

**Опции:**

| Опция | Короткая | Описание | По умолчанию |
|-------|----------|----------|--------------|
| `--port` | `-p` | Порт прослушивания | 1720 |
| `--tls-port` | — | TLS порт | 1300 |
| `--tls` | — | Включить TLS | false |
| `--max-calls` | `-m` | Макс. вызовов | 10 |
| `--scenario` | `-s` | Сценарий ответа | — |
| `--auto-answer` | — | Авто-ответ | false |
| `--help` | `-h` | Помощь | — |

**Примеры:**

```bash
# Базовое прослушивание
h323p listen

# С ограничением
h323p listen -m 50

# Со сценарием
h323p listen -s scenarios/callee.xml
```

---

### load

Нагрузочное тестирование.

```bash
h323p load <destination> [options]
```

**Аргументы:**

| Аргумент | Описание |
|----------|----------|
| `destination` | Адрес назначения |

**Опции:**

| Опция | Короткая | Описание | По умолчанию |
|-------|----------|----------|--------------|
| `--calls` | `-c` | Количество вызовов | 100 |
| `--rate` | `-r` | Вызовов в секунду | 10 |
| `--duration` | `-d` | Длительность теста (сек) | 0 |
| `--max-concurrent` | `-m` | Макс. одновременных | 50 |
| `--ramp-up` | — | Время нарастания (сек) | 0 |
| `--ramp-down` | — | Время спада (сек) | 0 |
| `--scenario` | `-s` | XML сценарий | — |
| `--destinations` | — | CSV файл назначений | — |
| `--cycle` | `-C` | Циклический перебор | false |
| `--stats-interval` | — | Интервал статистики (сек) | 5 |
| `--cdr` | — | CDR файл | — |
| `--report` | — | Файл отчёта | — |
| `--max-failures` | — | Макс. ошибок | 0 |
| `--failure-threshold` | — | Процент ошибок | 0.0 |
| `--help` | `-h` | Помощь | — |

**Примеры:**

```bash
# 100 вызовов, 10/сек
h323p load 192.168.1.100 -c 100 -r 10

# С рампами
h323p load 192.168.1.100 -c 500 -r 50 --ramp-up 10 --ramp-down 10

# С отчётом
h323p load 192.168.1.100 -c 100 -r 10 --report report.json --cdr calls.csv
```

---

### fuzz

Fuzzing тестирование.

```bash
h323p fuzz <destination> [options]
```

**Аргументы:**

| Аргумент | Описание |
|----------|----------|
| `destination` | Адрес назначения |

**Опции:**

| Опция | Короткая | Описание | По умолчанию |
|-------|----------|----------|--------------|
| `--type` | `-T` | Тип (rtp/rtcp/h225/h245) | rtp |
| `--percentage` | `-p` | Процент модификации | 50 |
| `--header-pct` | — | Процент заголовка | 50 |
| `--payload-pct` | — | Процент payload | 0 |
| `--seed` | — | Seed для генератора | random |
| `--max-packets` | — | Макс. пакетов | 0 |
| `--report` | — | Файл отчёта | — |
| `--help` | `-h` | Помощь | — |

**Примеры:**

```bash
# RTP fuzzing
h323p fuzz 192.168.1.100 -T rtp -p 50

# H.225 fuzzing
h323p fuzz 192.168.1.1 -T h225 -p 30

# С отчётом
h323p fuzz 192.168.1.100 -T rtp -p 50 --report fuzz_report.json
```

---

### register

Регистрация на Gatekeeper.

```bash
h323p register [options]
```

**Опции:**

| Опция | Короткая | Описание | По умолчанию |
|-------|----------|----------|--------------|
| `--gatekeeper` | `-g` | Gatekeeper адрес | — |
| `--username` | `-u` | Имя пользователя | — |
| `--password` | `-p` | Пароль | — |
| `--ttl` | — | Время регистрации (сек) | 300 |
| `--unregister` | — | Отписка | false |
| `--status` | — | Проверить статус | false |
| `--help` | `-h` | Помощь | — |

**Примеры:**

```bash
# Регистрация
h323p register -g 192.168.1.1 -u test -p secret

# Отписка
h323p register -g 192.168.1.1 --unregister
```

---

### info

Информация о системе.

```bash
h323p info [options]
```

**Опции:**

| Опция | Короткая | Описание |
|-------|----------|----------|
| `--version` | `-v` | Версия |
| `--codecs` | — | Список кодеков |
| `--features` | — | Возможности |
| `--all` | — | Вся информация |
| `--help` | `-h` | Помощь |

---

## Глобальные опции

| Опция | Описание |
|-------|----------|
| `--config` | Файл конфигурации |
| `--log-file` | Файл логов |
| `--log-level` | Уровень (debug/info/warn/error) |
| `--interface` | Локальный интерфейс |
| `--tcp-base` | Базовый TCP порт |
| `--udp-base` | Базовый UDP порт |
| `--rtp-base` | Базовый RTP порт |
| `--quiet` | Тихий режим |
| `--verbose` | Подробный режим |
| `--help` | Общая помощь |
```

---

### 7.3 Примеры сценариев

**Файл: `examples/scenarios/basic_call.xml`**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Basic Call" version="1.0">
  <description>Базовый тестовый вызов с аудио</description>
  
  <globals>
    <variable name="call_duration" value="30"/>
  </globals>
  
  <callflow name="caller">
    <actions>
      <setup destination="192.168.1.100:1720" faststart="true"/>
      
      <wait event="call_proceeding" timeout="5000" optional="true"/>
      
      <wait event="connect" timeout="30000">
        <on-success next="established"/>
        <on-timeout next="release"/>
      </wait>
      
      <label id="established"/>
      
      <open-channel type="audio" codec="G.711" direction="sendrecv"/>
      
      <pause distribution="fixed" value="${call_duration}000"/>
      
      <close-channel type="audio"/>
      
      <release cause="normal_call_clearing"/>
      
      <wait event="release_complete" timeout="5000"/>
      
      <log message="Call completed successfully"/>
    </actions>
  </callflow>
</scenario>
```

**Файл: `examples/scenarios/load_test.xml`**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="Load Test" version="1.0">
  <description>Нагрузочный тест с циклическим перебором</description>
  
  <globals>
    <variable name="max_calls" value="100"/>
    <variable name="call_duration" value="60"/>
    <variable name="destinations" value="192.168.1.100,192.168.1.101,192.168.1.102"/>
  </globals>
  
  <callflow name="load_generator">
    <actions>
      <loop variable="i" from="1" to="${max_calls}">
        <set_variable name="dest_index" value="${i} % 3"/>
        <set_variable name="current_dest" value="${destinations[${dest_index}]}"/>
        
        <setup destination="${current_dest}"/>
        
        <wait event="connect" timeout="30000">
          <on-success>
            <open-channel type="audio" codec="G.711"/>
            <pause distribution="normal" mean="${call_duration}000" stdev="10000"/>
            <close-channel type="audio"/>
            <release cause="normal_call_clearing"/>
            <log message="Call ${i} completed"/>
          </on-success>
          <on-failure>
            <log message="Call ${i} failed"/>
          </on-failure>
        </wait>
        
        <pause distribution="exponential" mean="1000"/>
      </loop>
      
      <wait event="all_calls_released" timeout="300000"/>
      
      <print-stats/>
    </actions>
  </callflow>
</scenario>
```

**Файл: `examples/scenarios/fuzzing.xml`**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<scenario name="RTP Fuzzing" version="1.0">
  <description>Fuzzing тест RTP пакетов</description>
  
  <globals>
    <variable name="fuzz_header_pct" value="50"/>
    <variable name="fuzz_payload_pct" value="10"/>
    <variable name="duration" value="60"/>
  </globals>
  
  <callflow name="fuzzer">
    <actions>
      <setup destination="192.168.1.100:1720"/>
      
      <wait event="connect" timeout="30000"/>
      
      <open-channel type="audio" codec="G.711">
        <fuzz enable="true"
              type="rtp"
              header_percentage="${fuzz_header_pct}"
              payload_percentage="${fuzz_payload_pct}"/>
      </open-channel>
      
      <pause distribution="fixed" value="${duration}000"/>
      
      <close-channel type="audio"/>
      <release cause="normal_call_clearing"/>
      
      <log message="Fuzzing session completed"/>
    </actions>
  </callflow>
</scenario>
```

---

### 7.4 CI/CD настройка

**Файл: `.github/workflows/build.yml`**

```yaml
name: Build

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

env:
  BUILD_TYPE: Release

jobs:
  build-linux:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          libssl-dev \
          libpugixml-dev \
          libgtest-dev
    
    - name: Build H323Plus
      run: |
        git clone https://github.com/willamowius/h323plus.git
        cd h323plus
        ./configure --enable-h235 --disable-sdl --disable-lua
        make debugnoshared
        echo "OPENH323DIR=$(pwd)" >> $GITHUB_ENV
        echo "PTLIBDIR=$(pwd)/../ptlib" >> $GITHUB_ENV
    
    - name: Configure CMake
      run: |
        cmake -B ${{github.workspace}}/build \
          -DCMAKE_BUILD_TYPE=${{env.BUILD_TYPE}} \
          -DH323PLUS_DIR=${{env.OPENH323DIR}} \
          -DBUILD_TESTS=ON
    
    - name: Build
      run: cmake --build ${{github.workspace}}/build --config ${{env.BUILD_TYPE}}
    
    - name: Test
      working-directory: ${{github.workspace}}/build
      run: ctest --build-config ${{env.BUILD_TYPE}} --output-on-failure
    
    - name: Upload binary
      uses: actions/upload-artifact@v3
      with:
        name: h323p-linux
        path: ${{github.workspace}}/build/h323p
```

**Файл: `.github/workflows/release.yml`**

```yaml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  create-release:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Build Changelog
      id: changelog
      uses: mikepenz/release-changelog-builder-action@v2
      with:
        configuration: ".github/changelog-config.json"
    
    - name: Create Release
      id: create_release
      uses: actions/create-release@v1
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
      with:
        tag_name: ${{ github.ref }}
        release_name: Release ${{ github.ref }}
        body: ${{steps.changelog.outputs.changelog}}
        draft: false
        prerelease: false
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 7

```
[ ] 1. README.md создан
[ ] 2. INSTALL.md создан
[ ] 3. CHANGELOG.md ведётся
[ ] 4. CLI справочник готов
[ ] 5. Примеры сценариев (10+) готовы
[ ] 6. CI/CD настроен
[ ] 7. Сборка проходит без ошибок
[ ] 8. Все тесты проходят
[ ] 9. Релиз v0.1.0 опубликован
[ ] 10. Документация на GitHub Pages
```

---

## 🧪 Проверка результатов

### Сборка

```bash
# Чистая сборка
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Проверка бинарника
./h323p --version
./h323p --help
```

### Тесты

```bash
# Все тесты
ctest --output-on-failure

# С покрытием
cmake .. -DCOVERAGE=ON
make coverage
./coverage.sh
```

### CI/CD

```bash
# Локальная проверка workflow
act -n  # Dry run

# Проверка сборки
act push
```

### Релиз

```bash
# Создание тега
git tag -a v0.1.0 -m "Release v0.1.0"
git push origin v0.1.0

# Проверка релиза
gh release view v0.1.0
```

---

## 📚 Ссылки

- **GitHub Actions:** https://docs.github.com/en/actions
- **CMake документация:** https://cmake.org/documentation/
- **Semantic Versioning:** https://semver.org/
- **Keep a Changelog:** https://keepachangelog.com/

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
