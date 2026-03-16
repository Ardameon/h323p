# H.323 Test Utility (h323p)

[![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0)

**h323p** — утилита для тестирования H.323 вызовов по аналогии с SIPP для SIP.

## ✨ Возможности

- 📞 **Нагрузочное тестирование** — до 1000+ одновременных вызовов
- 🎭 **Fuzzing тестирование** — RTP, H.225, H.245
- 📹 **Видео поддержка** — H.261, H.263, H.239 презентации
- 🔒 **TLS шифрование** — защищённая сигнализация
- 📊 **Статистика и отчёты** — JSON, CSV, HTML
- 📜 **XML сценарии** — гибкое описание тестов

## 📚 Документация

| Документ | Описание |
|----------|----------|
| [STAGE1.md](STAGE1.md) | Инфраструктура (CMake, CLI, логи) |
| [STAGE2.md](STAGE2.md) | H.323 стек (H.225, Call Manager) |
| [STAGE3.md](STAGE3.md) | H.245 и медиа (RTP, кодеки) |
| [STAGE4.md](STAGE4.md) | XML сценарии |
| [STAGE5.md](STAGE5.md) | Нагрузочное тестирование |
| [STAGE6.md](STAGE6.md) | Fuzzing, видео, H.239, TLS |
| [STAGE7.md](STAGE7.md) | Документация и релиз |
| [ARCH_PROJECT.md](ARCH_PROJECT.md) | Полный архитектурный документ |
| [QWEN.md](QWEN.md) | Краткий контекст проекта |

## 🚀 Быстрый старт

### Зависимости

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libssl-dev libpugixml-dev

# H323Plus (требуется сборка из исходников)
git clone https://github.com/willamowius/h323plus.git
cd h323plus
./configure --enable-h235
make debugnoshared
export OPENH323DIR=$(pwd)
```

### Сборка

```bash
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

## 📋 Команды

| Команда | Описание |
|---------|----------|
| `call` | Запуск H.323 вызова |
| `listen` | Режим прослушивания |
| `load` | Нагрузочное тестирование |
| `fuzz` | Fuzzing тестирование |
| `register` | Регистрация на Gatekeeper |
| `info` | Информация о системе |

## 📊 Статус проекта

**Фаза:** Проектирование завершено (100%)  
**Версия:** 1.0.0 (ready for implementation)  
**Готовность к реализации:** ✅

## 🔗 Ресурсы

- **H323Plus:** https://www.h323plus.org/
- **callgen323:** https://github.com/willamowius/callgen323
- **SIPP:** https://github.com/SIPp/sipp

## 📝 Лицензия

Распространяется под лицензией [MPL-2.0](LICENSE).

---

*Версия: 1.0.0 | Обновлено: 2026-03-16*
