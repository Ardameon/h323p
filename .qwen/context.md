# h323p — H.323 Test Utility

## Статус проекта
- **Фаза:** Проектирование завершено (100%)
- **Версия:** 1.0.0 (ready for implementation)
- **Последнее обновление:** 2026-03-16

## Ключевые решения
- **Язык:** C++17
- **H.323 стек:** H323Plus + PTLib
- **Сборка:** CMake
- **Сценарии:** XML (SIPP-подобный синтаксис)

## Структура проекта
```
h323p/
├── src/ (cli, core, scenario, h323, media, stats, utils)
├── scenarios/ (XML примеры)
├── tests/
└── docs/
```

## Документы по этапам
- `STAGE1.md` — Инфраструктура (CMake, CLI, логи, таймеры, unit тесты)
- `STAGE2.md` — H.323 стек (H.225 RAS/Q.931, Call Manager, call/listen)
- `STAGE3.md` — H.245 и медиа (RTP, кодеки, генератор)
- `STAGE4.md` — XML сценарии (парсер, интерпретатор, переменные)
- `STAGE5.md` — Нагрузочное тестирование (Call Pool, статистика, отчёты)
- `STAGE6.md` — Fuzzing, видео, H.239, TLS
- `STAGE7.md` — Документация и релиз (README, CLI, примеры, CI/CD)

## Следующий шаг
Начало реализации Этапа 1 (инфраструктура)

## Быстрые команды
```bash
./h323p call 192.168.1.100          # Базовый вызов
./h323p load 192.168.1.100 -c 100   # Нагрузка
./h323p listen                      # Прослушивание
./h323p fuzz 192.168.1.100 -T rtp   # Fuzzing
./h323p info --version              # Информация
```

## Ссылки
- **H323Plus:** https://www.h323plus.org/
- **callgen323:** https://github.com/willamowius/callgen323
- **SIPP:** https://github.com/SIPp/sipp
