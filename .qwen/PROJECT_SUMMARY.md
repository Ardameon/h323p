The user wants me to create a comprehensive project summary in markdown format based on the conversation history. This is a summary of the H.323 testing utility design session. I need to extract key decisions, architecture, plans, and knowledge from our conversation.

Let me organize this into the requested format:
1. Overall Goal - the main objective
2. Key Knowledge - technologies, architecture, decisions
3. Recent Actions - what was accomplished
4. Current Plan - roadmap with status markers

I'll write this in Russian as per the output language preference established in the context.# Project Summary: H.323 Testing Utility (h323p)

## Overall Goal

Проектирование и реализация утилиты для тестирования H.323 вызовов по аналогии с SIPP для SIP — нагрузочное тестирование, функциональное тестирование, fuzzing H.323 endpoints, gateways и gatekeepers.

---

## Key Knowledge

### Technology Stack
| Component | Choice | Rationale |
|-----------|--------|-----------|
| **Язык** | C++ | Совместимость с H323Plus |
| **H.323 стек** | H323Plus + PTLib | Единственная активно поддерживаемая open-source реализация |
| **Сборка** | CMake | Стандарт де-факто для C++ проектов |
| **CLI парсер** | CLI11 или boost::program_options | Современный/традиционный вариант |
| **Логирование** | spdlog | Быстрое, header-only |
| **Тесты** | Google Test / Catch2 | Популярные фреймворки |
| **XML парсер** | pugixml или tinyxml2 | Лёгкие, быстрые |
| **Лицензия** | MPL (как H323Plus/callgen323) | Совместимость |

### Альтернативы (отклонены)
- **ooh323c (C)** — мёртвый проект (~2010)
- **OPAL** — устарел, удалён из FreeBSD
- **Go/Rust/Python** — нет реализаций H.323

### Архитектурные решения
```
┌─────────────────────────────────────────────────────────┐
│                    h323p Utility                        │
├─────────────────────────────────────────────────────────┤
│  CLI Interface → Core Engine → H.323 Layer → Media     │
│                      (H323Plus Backend)                 │
└─────────────────────────────────────────────────────────┘
```

**Модули:**
- `cli/` — парсинг командной строки
- `core/` — Call Manager, Call, Endpoint
- `scenario/` — XML парсер, интерпретатор
- `h323/` — обёртки H.225 RAS, H.225 Q.931, H.245
- `media/` — RTP/RTCP, кодеки, генераторы
- `stats/` — статистика, CDR

### CLI Команды
| Команда | Назначение |
|---------|------------|
| `call` | Инициирование вызова |
| `listen` | Режим прослушивания |
| `load` | Нагрузочное тестирование |
| `fuzz` | Fuzzing тестирование |
| `register` | Регистрация на GK |
| `info` | Информация о системе |

### XML Сценарии (SIPP-подобные)
```xml
<scenario name="Basic Call">
  <callflow name="caller">
    <actions>
      <setup destination="192.168.1.100"/>
      <wait event="connect" timeout="30000"/>
      <open-channel type="audio" codec="G.711"/>
      <pause distribution="uniform" min="10000" max="30000"/>
      <close-channel type="audio"/>
      <release cause="normal_call_clearing"/>
    </actions>
  </callflow>
</scenario>
```

### Поддерживаемые функции (планируемые)
- **Протоколы:** H.225 RAS, H.225 Q.931, H.245
- **Медиа:** RTP/RTCP, G.711, G.729, H.264, H.263
- **Расширения:** H.235 (шифрование), H.239 (презентации), H.460.x (NAT traversal)
- **Режимы:** Dialing, Listening, Load Generation, Fuzzing

---

## Recent Actions

### Анализ существующих решений [DONE]
- Изучен **callgen323** (референсная реализация, C++, MPL)
- Изучен **H323Plus** (активно поддерживается, v1.28.0)
- Изучен **ooh323c** (C, мёртвый проект)
- Изучен **OPAL** (устарел)
- Изучен **SIPP** (XML сценарии, UAC/UAS режимы)

### Проектирование архитектуры [DONE]
- Спроектирована модульная структура проекта
- Определены компоненты: Call Manager, Scenario Engine, H.323 Wrapper, Media Layer
- Создана диаграмма состояний вызова

### Дизайн XML сценариев [DONE]
- Определён синтаксис по аналогии с SIPP
- Элементы: `<setup>`, `<wait>`, `<connect>`, `<open-channel>`, `<pause>`, `<release>`
- Поддержка переменных, CSV, распределений времени

### Проектирование CLI [DONE]
- 6 команд: call, listen, load, fuzz, register, info
- Глобальные опции и опции команд
- Примеры использования для каждой команды

### План реализации [DONE]
- 7 этапов, 13-17 недель total
- MVP: 8-10 недель (Этапы 1-3 + базовый Этап 5)

---

## Current Plan

### Roadmap

| Этап | Статус | Длительность | Ключевые результаты |
|------|--------|--------------|---------------------|
| **1. Инфраструктура** | [TODO] | 2-3 недели | CMake, CLI, логирование, тесты |
| **2. H.225 стек** | [TODO] | 3-4 недели | RAS, Q.931, вызовы без медиа |
| **3. H.245 и медиа** | [TODO] | 2-3 недели | Логические каналы, RTP, кодеки |
| **4. XML сценарии** | [TODO] | 2 недели | Парсер, интерпретатор, переменные |
| **5. Нагрузочное тестирование** | [TODO] | 2 недели | Множественные вызовы, статистика, CDR |
| **6. Fuzzing и расширения** | [TODO] | 1-2 недели | RTP/H.225 fuzzing, видео, H.239, TLS |
| **7. Документация и релиз** | [TODO] | 1 неделя | README, примеры, CI/CD, релиз 0.1.0 |

### MVP Scope (8-10 недель)
- [ ] Этап 1: Инфраструктура
- [ ] Этап 2: H.225 вызовы
- [ ] Этап 3: RTP медиа (базовый)
- [ ] Этап 5: Нагрузочное тестирование (базовое)

### Следующие шаги
1. Инициализация репозитория `h323p`
2. Создание CMakeLists.txt с зависимостями H323Plus/PTLib
3. Реализация CLI парсера и базовых команд
4. Сборка и тестирование на Linux

---

## Build & Dependencies

### Требования для сборки
```bash
# Зависимости
- gcc / clang
- CMake >= 3.10
- H323Plus (https://github.com/willamowius/h323plus)
- PTLib (https://github.com/willamowius/ptlib)
- OpenSSL dev

# Сборка зависимостей
git clone https://github.com/willamowius/ptlib.git
cd ptlib && ./configure --enable-ipv6 && make debugnoshared
export PTLIBDIR=$(pwd)

cd ..
git clone https://github.com/willamowius/h323plus.git
cd h323plus && ./configure --enable-h235 && make debugnoshared
export OPENH323DIR=$(pwd)
```

### Планируемая сборка проекта
```bash
mkdir build && cd build
cmake .. -DH323PLUS_DIR=$OPENH323DIR -DPTLIB_DIR=$PTLIBDIR
make -j4
./h323p info --version
```

---

## Reference Materials

| Resource | URL |
|----------|-----|
| callgen323 (референс) | https://github.com/willamowius/callgen323 |
| H323Plus | https://www.h323plus.org/ |
| PTLib | https://github.com/willamowius/ptlib |
| SIPP документация | https://sipp.readthedocs.io/ |
| H.323 стандарты | ITU-T H.225, H.245, H.235, H.239, H.460.x |

---

*Сессия завершена: полный анализ и проектирование выполнено. Готово к началу реализации Этапа 1.*

---

## Summary Metadata
**Update time**: 2026-03-16T06:14:26.552Z 
