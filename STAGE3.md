# Этап 3: H.245 и медиа

**Длительность:** 2-3 недели  
**Статус:** 📋 Готов к реализации  
**Зависимости:** ✅ Этап 1 (инфраструктура), ✅ Этап 2 (H.225 стек)

---

## 📋 Содержание

1. [Обзор этапа](#-обзор-этапа)
2. [Задачи этапа](#-задачи-этапа)
3. [Архитектура медиа слоя](#-архитектура-медиа-слоя)
4. [Детальная реализация](#-детальная-реализация)
5. [Диаграмма открытия каналов](#-диаграмма-открытия-каналов)
6. [Критерии завершения](#-критерии-завершения)
7. [Проверка результатов](#-проверка-результатов)

---

## 📌 Обзор этапа

Цель Этапа 3 — реализовать протокол H.245 для управления медиа каналами и добавить поддержку RTP/RTCP для передачи аудио.

### Результаты этапа

```
✅ H.245 протокол реализован
✅ Логические каналы открываются/закрываются
✅ RTP сессия для передачи медиа
✅ Кодеки (G.711, G.729)
✅ Генератор тестовых паттернов
✅ RTCP статистика
✅ Медиа интегрировано с вызовами
```

### Что НЕ входит в Этап 3

```
❌ Видео кодеки — Этап 6
❌ H.239 (презентации) — Этап 6
❌ XML сценарии — Этап 4
❌ Нагрузочное тестирование — Этап 5
```

---

## 📊 Задачи этапа

| № | Задача | Длительность | Статус |
|---|--------|--------------|--------|
| 3.1 | H.245 обёртка | 3 дня | ⏳ |
| 3.2 | Логические каналы | 4 дня | ⏳ |
| 3.3 | RTP сессия | 4 дня | ⏳ |
| 3.4 | Кодеки | 3 дня | ⏳ |
| 3.5 | Генератор медиа | 3 дня | ⏳ |
| 3.6 | RTCP | 2 дня | ⏳ |
| 3.7 | Тесты медиа | 3 дня | ⏳ |
| **Итого** | | **22 дня** | |

---

## 🏗️ Архитектура медиа слоя

### Общая схема

```
┌─────────────────────────────────────────────────────────────────┐
│                    Application Layer                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  Call Manager + H.225 (Этап 2)                          │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  H.245 Control Layer                      │ │
│  │  ┌─────────────────────────────────────────────────────┐ │ │
│  │  │  H245Manager (Master/Slave, Capabilities, Channels) │ │ │
│  │  └─────────────────────────────────────────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                    Media Layer                            │ │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────────┐ │ │
│  │  │ RTP Session  │  │Codec Manager │  │Media Generator  │ │ │
│  │  │              │  │              │  │                 │ │ │
│  │  │ - Send       │  │ - G.711      │  │ - Patterns      │ │ │
│  │  │ - Receive    │  │ - G.729      │  │ - File playback │ │ │
│  │  │ - Jitter buf │  │ - G.722      │  │ - File record   │ │ │
│  │  └──────────────┘  └──────────────┘  └─────────────────┘ │ │
│  └───────────────────────────────────────────────────────────┘ │
│                              │                                  │
│  ┌───────────────────────────┼───────────────────────────────┐ │
│  │                  H323Plus + PTLib                         │ │
│  │              (H.245 сообщения, транспорт)                 │ │
│  └───────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

### Структура модулей

```
src/
├── h323/
│   ├── h245.hpp                  # H.245 протокол
│   ├── h245.cpp
│   ├── h245_messages.hpp         # Типы сообщений H.245
│   └── h245_messages.cpp
├── media/
│   ├── rtp_session.hpp           # RTP сессия
│   ├── rtp_session.cpp
│   ├── rtp_packet.hpp            # RTP пакет
│   ├── rtp_packet.cpp
│   ├── rtcp_session.hpp          # RTCP статистика
│   ├── rtcp_session.cpp
│   ├── codec_manager.hpp         # Менеджер кодеков
│   ├── codec_manager.cpp
│   ├── codec.hpp                 # Интерфейс кодека
│   ├── codec.cpp
│   ├── codecs/
│   │   ├── g711_codec.hpp        # G.711 кодек
│   │   ├── g711_codec.cpp
│   │   ├── g729_codec.hpp        # G.729 кодек
│   │   └── g729_codec.cpp
│   ├── media_generator.hpp       # Генератор медиа
│   ├── media_generator.cpp
│   ├── media_patterns.hpp        # Тестовые паттерны
│   └── media_patterns.cpp
└── core/
    └── call.cpp                  # Обновлённый класс вызова
```

---

## 🔧 Детальная реализация

### 3.1 H.245 обёртка

**Файл: `src/h323/h245_messages.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace h323p {

// Типы сообщений H.245
enum class H245MessageType {
    // Запросы
    MASTER_SLAVE_DETERMINATION,
    MASTER_SLAVE_DETERMINATION_ACK,
    MASTER_SLAVE_DETERMINATION_REJECT,
    
    TERMINAL_CAPABILITY_SET,
    TERMINAL_CAPABILITY_SET_ACK,
    TERMINAL_CAPABILITY_SET_REJECT,
    
    OPEN_LOGICAL_CHANNEL,
    OPEN_LOGICAL_CHANNEL_ACK,
    OPEN_LOGICAL_CHANNEL_REJECT,
    
    CLOSE_LOGICAL_CHANNEL,
    CLOSE_LOGICAL_CHANNEL_ACK,
    
    REQUEST_CHANNEL_CLOSE,
    REQUEST_CHANNEL_CLOSE_ACK,
    
    END_SESSION_COMMAND,
    END_SESSION_RESPONSE,
    
    // Другие
    FLOW_CONTROL_COMMAND,
    MISCELLANEOUS_COMMAND
};

// Направление канала
enum class ChannelDirection {
    SEND,
    RECEIVE,
    SEND_RECEIVE
};

// Типы медиа
enum class MediaType {
    AUDIO,
    VIDEO,
    DATA,
    CONTROL
};

// Кодеки H.245
enum class H245Codec {
    G711_ULAW,    // G.711 μ-law (64 kbps)
    G711_ALAW,    // G.711 A-law (64 kbps)
    G729,         // G.729 (8 kbps)
    G7231,        // G.723.1 (5.3/6.3 kbps)
    G722,         // G.722 (48/56/64 kbps)
    G728,         // G.728 (16 kbps)
    G729A,        // G.729 Annex A (8 kbps)
    MPEG4_AUDIO,  // MPEG-4 Audio
    H261,         // H.261 video
    H263,         // H.263 video
    H264          // H.264 video
};

// Структура capabilities
struct Capability {
    H245Codec codec;
    uint32_t bitrate = 0;  // бит/сек
    uint32_t sampleRate = 0;  // Гц
    std::string extraParams;
};

// Структура логического канала
struct LogicalChannel {
    uint32_t channelNumber = 0;
    MediaType mediaType = MediaType::AUDIO;
    ChannelDirection direction = ChannelDirection::SEND_RECEIVE;
    Capability capability;
    
    // Транспортная информация
    std::string rtpAddress;
    int rtpPort = 0;
    int rtcpPort = 0;
    
    // Состояние
    bool isOpen = false;
    bool isLocal = true;  // true = локальный, false = удалённый
};

// Конвертация в строку
std::string h245MessageTypeToString(H245MessageType type);
std::string channelDirectionToString(ChannelDirection dir);
std::string mediaTypeToString(MediaType type);
std::string h245CodecToString(H245Codec codec);

} // namespace h323p
```

**Файл: `src/h323/h245.hpp`**

```cpp
#pragma once

#include "h245_messages.hpp"
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace h323p {

// Конфигурация H.245
struct H245Config {
    bool masterSlaveAuto = true;  // Авто-определение master/slave
    bool tunneling = true;  // Туннелирование через H.225
    int tcpPort = 0;  // 0 = динамический порт
    std::vector<Capability> capabilities;
};

// Состояние H.245 сессии
enum class H245State {
    IDLE,
    WAITING_MASTER_SLAVE,
    MASTER_SLAVE_DONE,
    WAITING_CAPABILITIES,
    CAPABILITIES_EXCHANGED,
    OPENING_CHANNELS,
    CHANNELS_OPEN,
    CLOSING,
    CLOSED
};

// H.245 Manager
class H245Manager {
public:
    H245Manager();
    ~H245Manager();
    
    // Инициализация
    bool initialize(const H245Config& config);
    void shutdown();
    
    // Состояние
    H245State getState() const { return state_; }
    bool isMaster() const { return isMaster_; }
    
    // Master/Slave определение
    bool sendMasterSlaveDetermination(uint32_t terminalType, uint32_t statusDeterminationNumber);
    bool handleMasterSlaveDetermination(uint32_t remoteTerminalType, uint32_t remoteStatusNumber);
    
    // Capabilities
    bool sendTerminalCapabilitySet(const std::vector<Capability>& caps);
    bool handleTerminalCapabilitySet(const std::vector<Capability>& remoteCaps);
    
    // Логические каналы
    uint32_t openLogicalChannel(const LogicalChannel& channel);
    bool closeLogicalChannel(uint32_t channelNumber);
    
    // Обработка входящих
    bool handleOpenLogicalChannel(const LogicalChannel& channel);
    bool handleCloseLogicalChannel(uint32_t channelNumber);
    
    // Callback
    using H245Callback = std::function<void(H245MessageType type, const std::string& data)>;
    void setCallback(H245Callback callback);
    
    // Каналы
    const std::map<uint32_t, LogicalChannel>& getLocalChannels() const { return localChannels_; }
    const std::map<uint32_t, LogicalChannel>& getRemoteChannels() const { return remoteChannels_; }
    LogicalChannel* getChannel(uint32_t channelNumber, bool local = true);
    
private:
    void setState(H245State state);
    uint32_t generateChannelNumber();
    
    H245Config config_;
    H245State state_ = H245State::IDLE;
    bool isMaster_ = true;
    uint32_t nextChannelNumber_ = 1;
    
    std::map<uint32_t, LogicalChannel> localChannels_;
    std::map<uint32_t, LogicalChannel> remoteChannels_;
    std::vector<Capability> localCapabilities_;
    std::vector<Capability> remoteCapabilities_;
    
    H245Callback callback_;
};

} // namespace h323p
```

**Файл: `src/h323/h245.cpp`**

```cpp
#include "h245.hpp"
#include "utils/logging.hpp"

namespace h323p {

H245Manager::H245Manager() = default;

H245Manager::~H245Manager() {
    shutdown();
}

bool H245Manager::initialize(const H245Config& config) {
    config_ = config;
    localCapabilities_ = config.capabilities;
    
    LOG_INFO("H.245 Manager инициализирован");
    LOG_INFO_FMT("Туннелирование: %s", config.tunneling ? "включено" : "отключено");
    LOG_INFO_FMT("Master/Slave авто: %s", config.masterSlaveAuto ? "включено" : "отключено");
    
    setState(H245State::IDLE);
    return true;
}

void H245Manager::shutdown() {
    LOG_INFO("Завершение H.245 Manager");
    
    // Закрытие всех каналов
    for (auto& [num, channel] : localChannels_) {
        if (channel.isOpen) {
            closeLogicalChannel(num);
        }
    }
    
    localChannels_.clear();
    remoteChannels_.clear();
    setState(H245State::CLOSED);
}

void H245Manager::setState(H245State state) {
    H245State oldState = state_;
    state_ = state;
    
    LOG_INFO_FMT("H.245 состояние: %d -> %d", static_cast<int>(oldState), static_cast<int>(state));
}

uint32_t H245Manager::generateChannelNumber() {
    return nextChannelNumber_++;
}

// Master/Slave определение

bool H245Manager::sendMasterSlaveDetermination(uint32_t terminalType, uint32_t statusNumber) {
    LOG_INFO_FMT("Отправка Master/Slave Determination (type=%u, status=%u)", 
                 terminalType, statusNumber);
    
    // Отправка сообщения H.245
    // Реализация зависит от API H323Plus
    
    if (callback_) {
        callback_(H245MessageType::MASTER_SLAVE_DETERMINATION, "");
    }
    
    setState(H245State::WAITING_MASTER_SLAVE);
    return true;
}

bool H245Manager::handleMasterSlaveDetermination(uint32_t remoteTerminalType, uint32_t remoteStatusNumber) {
    LOG_INFO_FMT("Получено Master/Slave Determination (type=%u, status=%u)", 
                 remoteTerminalType, remoteStatusNumber);
    
    // Логика определения master/slave
    // Terminal с меньшим значением становится slave
    // При равных terminalType сравнивается statusDeterminationNumber
    
    // Предполагаем, что мы master для примера
    isMaster_ = true;
    
    LOG_INFO_FMT("Результат: мы - %s", isMaster_ ? "MASTER" : "SLAVE");
    
    // Отправка ack
    if (callback_) {
        callback_(H245MessageType::MASTER_SLAVE_DETERMINATION_ACK, "");
    }
    
    setState(H245State::MASTER_SLAVE_DONE);
    return true;
}

// Capabilities

bool H245Manager::sendTerminalCapabilitySet(const std::vector<Capability>& caps) {
    LOG_INFO_FMT("Отправка Terminal Capability Set (%zu capabilities)", caps.size());
    
    for (const auto& cap : caps) {
        LOG_DEBUG_FMT("  - %s (%u bps, %u Hz)", 
                      h245CodecToString(cap.codec).c_str(),
                      cap.bitrate, cap.sampleRate);
    }
    
    if (callback_) {
        callback_(H245MessageType::TERMINAL_CAPABILITY_SET, "");
    }
    
    setState(H245State::WAITING_CAPABILITIES);
    return true;
}

bool H245Manager::handleTerminalCapabilitySet(const std::vector<Capability>& remoteCaps) {
    LOG_INFO_FMT("Получено Terminal Capability Set (%zu capabilities)", remoteCaps.size());
    
    remoteCapabilities_ = remoteCaps;
    
    // Отправка ack
    if (callback_) {
        callback_(H245MessageType::TERMINAL_CAPABILITY_SET_ACK, "");
    }
    
    setState(H245State::CAPABILITIES_EXCHANGED);
    return true;
}

// Логические каналы

uint32_t H245Manager::openLogicalChannel(const LogicalChannel& channel) {
    uint32_t channelNum = generateChannelNumber();
    
    LogicalChannel ch = channel;
    ch.channelNumber = channelNum;
    
    LOG_INFO_FMT("Открытие логического канала %u: %s %s",
                 channelNum,
                 mediaTypeToString(ch.mediaType).c_str(),
                 h245CodecToString(ch.capability.codec).c_str());
    LOG_INFO_FMT("  RTP: %s:%d, RTCP: %s:%d",
                 ch.rtpAddress.c_str(), ch.rtpPort,
                 ch.rtpAddress.c_str(), ch.rtcpPort);
    
    localChannels_[channelNum] = ch;
    
    if (callback_) {
        callback_(H245MessageType::OPEN_LOGICAL_CHANNEL, std::to_string(channelNum));
    }
    
    if (state_ == H245State::CAPABILITIES_EXCHANGED) {
        setState(H245State::OPENING_CHANNELS);
    }
    
    return channelNum;
}

bool H245Manager::closeLogicalChannel(uint32_t channelNumber) {
    auto it = localChannels_.find(channelNumber);
    if (it == localChannels_.end()) {
        LOG_ERROR_FMT("Канал %u не найден", channelNumber);
        return false;
    }
    
    LOG_INFO_FMT("Закрытие логического канала %u", channelNumber);
    
    it->second.isOpen = false;
    
    if (callback_) {
        callback_(H245MessageType::CLOSE_LOGICAL_CHANNEL, std::to_string(channelNumber));
    }
    
    return true;
}

bool H245Manager::handleOpenLogicalChannel(const LogicalChannel& channel) {
    LOG_INFO_FMT("Входящий логический канал %u: %s",
                 channel.channelNumber,
                 h245CodecToString(channel.capability.codec).c_str());
    
    remoteChannels_[channel.channelNumber] = channel;
    remoteChannels_[channel.channelNumber].isOpen = true;
    
    if (callback_) {
        callback_(H245MessageType::OPEN_LOGICAL_CHANNEL_ACK, 
                  std::to_string(channel.channelNumber));
    }
    
    return true;
}

bool H245Manager::handleCloseLogicalChannel(uint32_t channelNumber) {
    auto it = remoteChannels_.find(channelNumber);
    if (it == remoteChannels_.end()) {
        return false;
    }
    
    LOG_INFO_FMT("Удалённое закрытие канала %u", channelNumber);
    
    it->second.isOpen = false;
    
    if (callback_) {
        callback_(H245MessageType::CLOSE_LOGICAL_CHANNEL_ACK, 
                  std::to_string(channelNumber));
    }
    
    return true;
}

LogicalChannel* H245Manager::getChannel(uint32_t channelNumber, bool local) {
    auto& channels = local ? localChannels_ : remoteChannels_;
    auto it = channels.find(channelNumber);
    if (it != channels.end()) {
        return &it->second;
    }
    return nullptr;
}

void H245Manager::setCallback(H245Callback callback) {
    callback_ = callback;
}

// Конвертация в строку

std::string h245MessageTypeToString(H245MessageType type) {
    switch (type) {
        case H245MessageType::MASTER_SLAVE_DETERMINATION: return "MasterSlaveDetermination";
        case H245MessageType::OPEN_LOGICAL_CHANNEL: return "OpenLogicalChannel";
        case H245MessageType::CLOSE_LOGICAL_CHANNEL: return "CloseLogicalChannel";
        default: return "Unknown";
    }
}

std::string channelDirectionToString(ChannelDirection dir) {
    switch (dir) {
        case ChannelDirection::SEND: return "send";
        case ChannelDirection::RECEIVE: return "receive";
        case ChannelDirection::SEND_RECEIVE: return "sendReceive";
        default: return "unknown";
    }
}

std::string mediaTypeToString(MediaType type) {
    switch (type) {
        case MediaType::AUDIO: return "audio";
        case MediaType::VIDEO: return "video";
        case MediaType::DATA: return "data";
        case MediaType::CONTROL: return "control";
        default: return "unknown";
    }
}

std::string h245CodecToString(H245Codec codec) {
    switch (codec) {
        case H245Codec::G711_ULAW: return "G.711 μ-law";
        case H245Codec::G711_ALAW: return "G.711 A-law";
        case H245Codec::G729: return "G.729";
        case H245Codec::G722: return "G.722";
        case H245Codec::H261: return "H.261";
        case H245Codec::H264: return "H.264";
        default: return "Unknown";
    }
}

} // namespace h323p
```

---

### 3.2 RTP сессия

**Файл: `src/media/rtp_packet.hpp`**

```cpp
#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace h323p {

// RTP заголовок (RFC 3550)
struct RtpHeader {
    uint8_t version : 2;        // Версия (2)
    uint8_t padding : 1;        // Padding
    uint8_t extension : 1;      // Extension
    uint8_t csrcCount : 4;      // CSRC count
    uint8_t marker : 1;         // Marker
    uint8_t payloadType : 7;    // Payload type
    uint16_t sequenceNumber;    // Sequence number
    uint32_t timestamp;         // Timestamp
    uint32_t ssrc;              // SSRC
    std::vector<uint32_t> csrc; // CSRC list (optional)
};

// RTP пакет
class RtpPacket {
public:
    RtpPacket();
    explicit RtpPacket(size_t payloadSize);
    
    // Сериализация
    std::vector<uint8_t> serialize() const;
    bool deserialize(const uint8_t* data, size_t size);
    
    // Доступ к заголовку
    RtpHeader& header() { return header_; }
    const RtpHeader& header() const { return header_; }
    
    // Доступ к payload
    uint8_t* payload() { return payload_.data(); }
    const uint8_t* payload() const { return payload_.data(); }
    size_t payloadSize() const { return payload_.size(); }
    void setPayload(const uint8_t* data, size_t size);
    
    // Размеры
    size_t totalSize() const { return 12 + header_.csrcCount * 4 + payload_.size(); }
    
private:
    RtpHeader header_;
    std::vector<uint8_t> payload_;
};

} // namespace h323p
```

**Файл: `src/media/rtp_session.hpp`**

```cpp
#pragma once

#include "rtp_packet.hpp"
#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>

namespace h323p {

// Конфигурация RTP сессии
struct RtpSessionConfig {
    std::string localAddress = "0.0.0.0";
    int localRtpPort = 0;  // 0 = динамический
    int localRtcpPort = 0; // 0 = RTP порт + 1
    
    std::string remoteAddress;
    int remoteRtpPort = 0;
    int remoteRtcpPort = 0;
    
    int payloadType = 0;  // PT для кодека
    uint32_t ssrc = 0;    // 0 = случайный
    int ttl = 64;
    
    // Jitter buffer
    int jitterBufferSize = 50;  // мс
};

// RTP статистика
struct RtpStatistics {
    uint64_t packetsSent = 0;
    uint64_t packetsReceived = 0;
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint32_t packetsLost = 0;
    uint32_t jitter = 0;  // мс
    double roundTripTime = 0.0;  // мс
};

// RTP сессия
class RtpSession {
public:
    RtpSession();
    ~RtpSession();
    
    // Инициализация
    bool initialize(const RtpSessionConfig& config);
    void shutdown();
    bool isInitialized() const { return initialized_; }
    
    // Отправка
    bool sendPacket(const RtpPacket& packet);
    bool sendPayload(const uint8_t* data, size_t size, uint32_t timestamp, bool marker = false);
    
    // Получение
    using PacketCallback = std::function<void(const RtpPacket&)>;
    void setReceiveCallback(PacketCallback callback);
    
    // Состояние
    bool isSending() const { return sending_; }
    bool isReceiving() const { return receiving_; }
    
    void startSending();
    void stopSending();
    void startReceiving();
    void stopReceiving();
    
    // Статистика
    RtpStatistics getStatistics() const;
    void resetStatistics();
    
    // SSRC
    uint32_t getSsrc() const { return config_.ssrc; }
    uint32_t getRemoteSsrc() const { return remoteSsrc_; }
    
    // Порты
    int getLocalRtpPort() const { return localRtpPort_; }
    int getLocalRtcpPort() const { return localRtcpPort_; }
    
private:
    void receiveThread();
    void rtcpThread();
    
    RtpSessionConfig config_;
    RtpStatistics stats_;
    
    int rtpSocket_ = -1;
    int rtcpSocket_ = -1;
    int localRtpPort_ = 0;
    int localRtcpPort_ = 0;
    
    std::atomic<bool> initialized_{false};
    std::atomic<bool> sending_{false};
    std::atomic<bool> receiving_{false};
    
    std::thread receiveThread_;
    std::thread rtcpThread_;
    
    PacketCallback receiveCallback_;
    
    // Sequence
    uint16_t sequenceNumber_ = 0;
    uint32_t timestamp_ = 0;
    uint32_t remoteSsrc_ = 0;
    
    // Jitter buffer
    std::queue<RtpPacket> jitterBuffer_;
    std::mutex jitterMutex_;
};

} // namespace h323p
```

**Файл: `src/media/rtp_session.cpp`**

```cpp
#include "rtp_session.hpp"
#include "utils/logging.hpp"
#include <cstring>
#include <random>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

namespace h323p {

RtpSession::RtpSession() = default;

RtpSession::~RtpSession() {
    shutdown();
}

bool RtpSession::initialize(const RtpSessionConfig& config) {
    config_ = config;
    
    LOG_INFO("Инициализация RTP сессии");
    
    // Генерация SSRC если не указан
    if (config_.ssrc == 0) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 0xFFFFFFFF);
        config_.ssrc = dis(gen);
    }
    
    // Создание RTP сокета
    rtpSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (rtpSocket_ < 0) {
        LOG_ERROR("Не удалось создать RTP сокет");
        return false;
    }
    
    // Настройка сокета
    int reuse = 1;
    setsockopt(rtpSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // Bind к локальному порту
    struct sockaddr_in localAddr = {};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = inet_addr(config.localAddress.c_str());
    
    // Поиск свободного порта
    int basePort = config.localRtpPort > 0 ? config.localRtpPort : 10000;
    for (int port = basePort; port < basePort + 1000; port += 2) {
        localAddr.sin_port = htons(port);
        if (bind(rtpSocket_, (struct sockaddr*)&localAddr, sizeof(localAddr)) == 0) {
            localRtpPort_ = port;
            localRtcpPort_ = port + 1;
            break;
        }
    }
    
    if (localRtpPort_ == 0) {
        LOG_ERROR("Не удалось найти свободный порт");
        close(rtpSocket_);
        return false;
    }
    
    LOG_INFO_FMT("RTP сокет: %s:%d", config.localAddress.c_str(), localRtpPort_);
    LOG_INFO_FMT("RTCP сокет: %s:%d", config.localAddress.c_str(), localRtcpPort_);
    
    // Создание RTCP сокета
    rtcpSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (rtcpSocket_ < 0) {
        LOG_ERROR("Не удалось создать RTCP сокет");
        close(rtpSocket_);
        return false;
    }
    
    setsockopt(rtcpSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    struct sockaddr_in rtcpAddr = {};
    rtcpAddr.sin_family = AF_INET;
    rtcpAddr.sin_addr.s_addr = inet_addr(config.localAddress.c_str());
    rtcpAddr.sin_port = htons(localRtcpPort_);
    
    if (bind(rtcpSocket_, (struct sockaddr*)&rtcpAddr, sizeof(rtcpAddr)) < 0) {
        LOG_ERROR("Не удалось bind RTCP сокет");
        close(rtpSocket_);
        close(rtcpSocket_);
        return false;
    }
    
    // Non-blocking mode
    int flags = fcntl(rtpSocket_, F_GETFL, 0);
    fcntl(rtpSocket_, F_SETFL, flags | O_NONBLOCK);
    
    flags = fcntl(rtcpSocket_, F_GETFL, 0);
    fcntl(rtcpSocket_, F_SETFL, flags | O_NONBLOCK);
    
    initialized_ = true;
    LOG_INFO("RTP сессия инициализирована");
    
    return true;
}

void RtpSession::shutdown() {
    if (!initialized_) return;
    
    LOG_INFO("Завершение RTP сессии");
    
    stopSending();
    stopReceiving();
    
    if (rtpSocket_ >= 0) {
        close(rtpSocket_);
        rtpSocket_ = -1;
    }
    
    if (rtcpSocket_ >= 0) {
        close(rtcpSocket_);
        rtcpSocket_ = -1;
    }
    
    initialized_ = false;
}

bool RtpSession::sendPacket(const RtpPacket& packet) {
    if (!initialized_ || !sending_) {
        return false;
    }
    
    std::vector<uint8_t> data = packet.serialize();
    
    struct sockaddr_in remoteAddr = {};
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = inet_addr(config_.remoteAddress.c_str());
    remoteAddr.sin_port = htons(config_.remoteRtpPort);
    
    ssize_t sent = sendto(rtpSocket_, data.data(), data.size(), 0,
                          (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
    
    if (sent > 0) {
        stats_.packetsSent++;
        stats_.bytesSent += sent;
    }
    
    return sent > 0;
}

bool RtpSession::sendPayload(const uint8_t* data, size_t size, uint32_t timestamp, bool marker) {
    RtpPacket packet(size);
    
    auto& header = packet.header();
    header.version = 2;
    header.padding = 0;
    header.extension = 0;
    header.csrcCount = 0;
    header.marker = marker ? 1 : 0;
    header.payloadType = config_.payloadType;
    header.sequenceNumber = htons(sequenceNumber_++);
    header.timestamp = htonl(timestamp);
    header.ssrc = htonl(config_.ssrc);
    
    packet.setPayload(data, size);
    
    return sendPacket(packet);
}

void RtpSession::setReceiveCallback(PacketCallback callback) {
    receiveCallback_ = callback;
}

void RtpSession::startSending() {
    sending_ = true;
    LOG_INFO("RTP отправка запущена");
}

void RtpSession::stopSending() {
    sending_ = false;
    LOG_INFO("RTP отправка остановлена");
}

void RtpSession::startReceiving() {
    if (receiving_) return;
    
    receiving_ = true;
    receiveThread_ = std::thread(&RtpSession::receiveThread, this);
    rtcpThread_ = std::thread(&RtpSession::rtcpThread, this);
    
    LOG_INFO("RTP приём запущен");
}

void RtpSession::stopReceiving() {
    if (!receiving_) return;
    
    receiving_ = false;
    
    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }
    
    if (rtcpThread_.joinable()) {
        rtcpThread_.join();
    }
    
    LOG_INFO("RTP приём остановлен");
}

void RtpSession::receiveThread() {
    uint8_t buffer[2048];
    
    while (receiving_) {
        struct sockaddr_in remoteAddr = {};
        socklen_t addrLen = sizeof(remoteAddr);
        
        ssize_t received = recvfrom(rtpSocket_, buffer, sizeof(buffer), 0,
                                    (struct sockaddr*)&remoteAddr, &addrLen);
        
        if (received > 12) {  // Минимальный RTP заголовок
            RtpPacket packet;
            if (packet.deserialize(buffer, received)) {
                stats_.packetsReceived++;
                stats_.bytesReceived += received;
                
                if (receiveCallback_) {
                    receiveCallback_(packet);
                }
            }
        }
    }
}

void RtpSession::rtcpThread() {
    // Упрощённая RTCP логика
    // В реальной реализации нужно отправлять SR/RR пакеты
    
    while (receiving_) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Отправка Receiver Report
        // ...
    }
}

RtpStatistics RtpSession::getStatistics() const {
    return stats_;
}

void RtpSession::resetStatistics() {
    stats_ = RtpStatistics();
}

} // namespace h323p
```

---

### 3.3 Менеджер кодеков

**Файл: `src/media/codec.hpp`**

```cpp
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace h323p {

// Тип кодека
enum class CodecType {
    G711_ULAW,
    G711_ALAW,
    G729,
    G7231,
    G722,
    G728,
    G729A
};

// Информация о кодеке
struct CodecInfo {
    CodecType type;
    std::string name;
    int sampleRate = 8000;  // Гц
    int bitrate = 0;        // бит/сек
    int frameSize = 0;      // сэмплов в фрейме
    int payloadSize = 0;    // байт в фрейме
    uint8_t payloadType = 0;  // RTP payload type
};

// Базовый класс кодека
class Codec {
public:
    virtual ~Codec() = default;
    
    // Информация
    virtual CodecInfo getInfo() const = 0;
    
    // Кодирование
    virtual bool encode(const int16_t* input, size_t inputSize, 
                        uint8_t* output, size_t& outputSize) = 0;
    
    // Декодирование
    virtual bool decode(const uint8_t* input, size_t inputSize,
                        int16_t* output, size_t& outputSize) = 0;
    
    // Свойства
    virtual int getSampleRate() const = 0;
    virtual int getFrameSize() const = 0;
    virtual int getBitrate() const = 0;
};

// Фабрика кодеков
class CodecFactory {
public:
    static Codec* create(CodecType type);
    static CodecInfo getCodecInfo(CodecType type);
    static std::vector<CodecInfo> getSupportedCodecs();
};

} // namespace h323p
```

**Файл: `src/media/codecs/g711_codec.hpp`**

```cpp
#pragma once

#include "codec.hpp"

namespace h323p {

// G.711 кодек (A-law и μ-law)
class G711Codec : public Codec {
public:
    explicit G711Codec(bool ulaw = false);
    
    CodecInfo getInfo() const override;
    
    bool encode(const int16_t* input, size_t inputSize,
                uint8_t* output, size_t& outputSize) override;
    
    bool decode(const uint8_t* input, size_t inputSize,
                int16_t* output, size_t& outputSize) override;
    
    int getSampleRate() const override { return 8000; }
    int getFrameSize() const override { return 80; }  // 10ms @ 8kHz
    int getBitrate() const override { return 64000; }
    
private:
    bool ulaw_;
    
    // Конвертация
    uint8_t linearToAlaw(int16_t sample);
    uint8_t linearToUlaw(int16_t sample);
    int16_t alawToLinear(uint8_t code);
    int16_t ulawToLinear(uint8_t code);
};

} // namespace h323p
```

**Файл: `src/media/codec_manager.hpp`**

```cpp
#pragma once

#include "codec.hpp"
#include <map>
#include <memory>
#include <vector>

namespace h323p {

// Менеджер кодеков
class CodecManager {
public:
    static CodecManager& instance();
    
    // Инициализация
    void initialize();
    void shutdown();
    
    // Получение кодека
    Codec* getCodec(CodecType type);
    
    // Доступные кодеки
    std::vector<CodecInfo> getSupportedCodecs() const;
    
    // Выбор лучшего кодека из предложенных
    CodecType selectBestCodec(const std::vector<CodecType>& remoteCodecs);
    
    // RTP payload type
    uint8_t getPayloadType(CodecType type) const;
    CodecType getCodecByPayloadType(uint8_t pt) const;
    
private:
    CodecManager() = default;
    ~CodecManager();
    
    CodecManager(const CodecManager&) = delete;
    CodecManager& operator=(const CodecManager&) = delete;
    
    std::map<CodecType, std::unique_ptr<Codec>> codecs_;
    std::map<CodecType, uint8_t> payloadTypes_;
    bool initialized_ = false;
};

} // namespace h323p
```

---

### 3.4 Генератор медиа

**Файл: `src/media/media_patterns.hpp`**

```cpp
#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace h323p {

// Типы паттернов
enum class PatternType {
    SILENCE,          // Тишина
    TONE,             // Тон (синусоида)
    DUAL_TONE,        // Два тона (DTMF-подобный)
    WHITE_NOISE,      // Белый шум
    BOUNCING_BOXES,   // "Скачущие коробки" (аудио аналог)
    MOVING_BLOCKS,    // "Движущиеся блоки"
    SPEECH_SIMULATION // Симуляция речи
};

// Конфигурация генератора
struct PatternConfig {
    PatternType type = PatternType::TONE;
    int sampleRate = 8000;
    int channels = 1;
    double frequency = 440.0;  // Гц для тона
    double frequency2 = 480.0; // Второй тон для dual-tone
    double amplitude = 0.5;    // 0.0 - 1.0
    bool autoGain = false;
};

// Генератор паттернов
class MediaPatternGenerator {
public:
    MediaPatternGenerator();
    
    // Настройка
    void setConfig(const PatternConfig& config);
    
    // Генерация
    void generate(int16_t* buffer, size_t samples);
    
    // Сброс
    void reset();
    
private:
    PatternConfig config_;
    size_t sampleIndex_ = 0;
    double phase_ = 0.0;
    double phase2_ = 0.0;
};

} // namespace h323p
```

**Файл: `src/media/media_generator.hpp`**

```cpp
#pragma once

#include "rtp_session.hpp"
#include "codec.hpp"
#include "media_patterns.hpp"
#include <string>
#include <atomic>
#include <thread>
#include <fstream>

namespace h323p {

// Конфигурация генератора медиа
struct MediaGeneratorConfig {
    CodecType codecType = CodecType::G711_ULAW;
    PatternType patternType = PatternType::TONE;
    int frameDuration = 20;  // мс
    std::string inputFile;   // WAV файл для воспроизведения
    std::string outputFile;  // WAV файл для записи
    bool loopFile = false;
};

// Медиа генератор
class MediaGenerator {
public:
    MediaGenerator();
    ~MediaGenerator();
    
    // Инициализация
    bool initialize(const MediaGeneratorConfig& config, RtpSession* rtpSession);
    void shutdown();
    
    // Запуск/остановка
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Статистика
    uint64_t getFramesSent() const { return framesSent_; }
    uint64_t getFramesReceived() const { return framesReceived_; }
    double getDuration() const;  // секунды
    
private:
    void sendThread();
    void receiveThread();
    void generateFrame(uint8_t* output, size_t& size);
    
    MediaGeneratorConfig config_;
    RtpSession* rtpSession_ = nullptr;
    Codec* codec_ = nullptr;
    MediaPatternGenerator patternGenerator_;
    
    std::atomic<bool> running_{false};
    std::thread sendThread_;
    std::thread receiveThread_;
    
    uint64_t framesSent_ = 0;
    uint64_t framesReceived_ = 0;
    uint32_t timestamp_ = 0;
    
    std::ifstream inputFile_;
    std::ofstream outputFile_;
};

} // namespace h323p
```

---

### 3.5 Интеграция с вызовами

**Обновление `src/core/call.hpp`**

```cpp
#pragma once

#include "h323/h245.hpp"
#include "media/rtp_session.hpp"
#include "media/media_generator.hpp"
#include <string>
#include <memory>

namespace h323p {

// Расширенное состояние вызова
enum class ExtendedCallState {
    // H.225 состояния (из Этапа 2)
    CALL_NULL = 0,
    CALL_INITIATED,
    CALL_RECEIVED,
    CALL_PROCEEDING,
    CALL_ALERTING,
    CALL_ACTIVE,
    CALL_RELEASED,
    
    // H.245 состояния
    H245_STARTING,
    H245_MASTER_SLAVE,
    H245_CAPABILITIES,
    H245_OPENING_CHANNELS,
    H245_CHANNELS_OPEN,
    
    // Медиа состояния
    MEDIA_STARTING,
    MEDIA_ACTIVE,
    MEDIA_STOPPED
};

// Конфигурация медиа для вызова
struct CallMediaConfig {
    bool enableAudio = true;
    bool enableVideo = false;
    CodecType audioCodec = CodecType::G711_ULAW;
    int audioBitrate = 64000;
    PatternType patternType = PatternType::TONE;
    double toneFrequency = 440.0;
};

// Расширенный класс вызова
class Call {
public:
    Call(const std::string& callId, bool incoming = false);
    ~Call();
    
    // Основная информация
    const std::string& getCallId() const { return callId_; }
    bool isIncoming() const { return incoming_; }
    ExtendedCallState getState() const { return state_; }
    
    // H.245
    H245Manager& getH245() { return h245_; }
    
    // Медиа
    RtpSession* getRtpSession() { return rtpSession_.get(); }
    MediaGenerator* getMediaGenerator() { return mediaGenerator_.get(); }
    
    // Управление состоянием
    void setState(ExtendedCallState state);
    
    // H.245 процессы
    bool startH245();
    bool openAudioChannel();
    bool closeAudioChannel();
    
    // Медиа процессы
    bool startMedia();
    bool stopMedia();
    
    // Статистика
    struct CallStats {
        double duration = 0.0;  // секунды
        uint64_t rtpPacketsSent = 0;
        uint64_t rtpPacketsReceived = 0;
        uint32_t jitter = 0;
    };
    CallStats getStats() const;
    
private:
    void onH245Event(H245MessageType type, const std::string& data);
    
    std::string callId_;
    bool incoming_;
    ExtendedCallState state_ = ExtendedCallState::CALL_NULL;
    
    H245Manager h245_;
    std::unique_ptr<RtpSession> rtpSession_;
    std::unique_ptr<MediaGenerator> mediaGenerator_;
    
    CallMediaConfig mediaConfig_;
};

} // namespace h323p
```

---

## 📊 Диаграмма открытия каналов

```
┌─────────────────────────────────────────────────────────────────┐
│                    Call Established (H.225)                     │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│ 1. Master/Slave Determination                                   │
│    ┌──────────────┐         ┌──────────────┐                   │
│    │   Master     │         │    Slave     │                   │
│    │   MSD ──────►│         │◄────── MSD   │                   │
│    │◄────── MSDA  │         │   MSDA ─────►│                   │
│    └──────────────┘         └──────────────┘                   │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│ 2. Capability Exchange                                          │
│    ┌──────────────┐         ┌──────────────┐                   │
│    │   Master     │         │    Slave     │                   │
│    │   TCS ──────►│         │◄────── TCS   │                   │
│    │◄────── TCSA  │         │   TCSA ─────►│                   │
│    └──────────────┘         └──────────────┘                   │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│ 3. Open Logical Channel (Audio)                                 │
│    ┌──────────────┐         ┌──────────────┐                   │
│    │   Master     │         │    Slave     │                   │
│    │   OLC ──────►│         │              │                   │
│    │              │         │   OLCA ─────►│                   │
│    │              │ RTP/RTCP │              │                   │
│    │◄══════════════════════════════════════►│                   │
│    └──────────────┘         └──────────────┘                   │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│ 4. Media Active                                                 │
│    ┌──────────────┐         ┌──────────────┐                   │
│    │   Master     │         │    Slave     │                   │
│    │  RTP Send ──►│         │◄── RTP Recv  │                   │
│    │◄── RTP Recv  │         │   RTP Send ─►│                   │
│    │  RTCP Recv ◄─│         │─► RTCP Send  │                   │
│    └──────────────┘         └──────────────┘                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## ✅ Критерии завершения

### Чек-лист завершения Этапа 3

```
[ ] 1. H.245 Manager реализован
[ ] 2. Master/Slave определение работает
[ ] 3. Capability exchange работает
[ ] 4. Логические каналы открываются/закрываются
[ ] 5. RTP сессия отправляет/получает пакеты
[ ] 6. G.711 кодек работает
[ ] 7. Генератор паттернов работает
[ ] 8. RTCP статистика собирается
[ ] 9. Медиа интегрировано с вызовами
[ ] 10. Тесты медиа проходят
```

---

## 🔧 Рекомендации по улучшению (из RECOMMENDATIONS.md)

### Рекомендация #8: Jitter buffer с сортировкой

**Приоритет:** 🟡 Важный  
**Сложность:** Высокая

**Проблема:** `std::queue<RtpPacket>` без сортировки по sequence number.

**Решение:**
```cpp
// src/media/jitter_buffer.hpp
#pragma once

#include "rtp_packet.hpp"
#include <map>
#include <optional>
#include <chrono>

namespace h323p {

class JitterBuffer {
public:
    explicit JitterBuffer(size_t maxPackets = 100, size_t maxDelayMs = 200);
    
    // Добавление пакета
    void push(const RtpPacket& packet);
    
    // Получение следующего пакета (в порядке)
    std::optional<RtpPacket> pop();
    
    // Статистика
    struct Stats {
        size_t packetsReceived = 0;
        size_t packetsDelivered = 0;
        size_t packetsLost = 0;
        size_t packetsOutOfOrder = 0;
        uint32_t currentJitterMs = 0;
    };
    
    Stats getStats() const;
    void reset();
    
private:
    // Хранение по sequence number для сортировки
    std::map<uint16_t, RtpPacket> packets_;
    
    size_t maxPackets_;
    size_t maxDelayMs_;
    
    uint16_t expectedSeq_ = 0;
    bool initialized_ = false;
    
    Stats stats_;
    
    std::chrono::steady_clock::time_point lastPushTime_;
    uint32_t lastTimestamp_ = 0;
};

} // namespace h323p
```

**Реализация:**
```cpp
// src/media/jitter_buffer.cpp
#include "jitter_buffer.hpp"

JitterBuffer::JitterBuffer(size_t maxPackets, size_t maxDelayMs)
    : maxPackets_(maxPackets), maxDelayMs_(maxDelayMs) {}

void JitterBuffer::push(const RtpPacket& packet) {
    auto now = std::chrono::steady_clock::now();
    uint16_t seq = packet.header().sequenceNumber;
    
    stats_.packetsReceived++;
    
    // Первый пакет
    if (!initialized_) {
        expectedSeq_ = seq;
        initialized_ = true;
    }
    
    // Проверка на дубликат
    if (packets_.find(seq) != packets_.end()) {
        return;
    }
    
    // Проверка на устаревший пакет
    if (seq < expectedSeq_) {
        stats_.packetsOutOfOrder++;
        return;
    }
    
    // Добавление в буфер
    packets_[seq] = packet;
    
    // Очистка старых пакетов при переполнении
    while (packets_.size() > maxPackets_) {
        packets_.erase(packets_.begin());
        stats_.packetsLost++;
    }
    
    // Расчёт джиттера
    if (lastTimestamp_ != 0) {
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastPushTime_).count();
        stats_.currentJitterMs = (stats_.currentJitterMs * 7 + delta) / 8;
    }
    
    lastPushTime_ = now;
    lastTimestamp_ = packet.header().timestamp;
}

std::optional<RtpPacket> JitterBuffer::pop() {
    auto it = packets_.find(expectedSeq_);
    if (it == packets_.end()) {
        // Ожидание пакета
        return std::nullopt;
    }
    
    RtpPacket packet = it->second;
    packets_.erase(it);
    
    expectedSeq_++;
    stats_.packetsDelivered++;
    
    return packet;
}

JitterBuffer::Stats JitterBuffer::getStats() const {
    return stats_;
}

void JitterBuffer::reset() {
    packets_.clear();
    stats_ = Stats();
    initialized_ = false;
}
```

**Использование в RtpSession:**
```cpp
// src/media/rtp_session.hpp
#include "jitter_buffer.hpp"

class RtpSession {
private:
    JitterBuffer jitterBuffer_;
};

// src/media/rtp_session.cpp
void RtpSession::receiveThread() {
    uint8_t buffer[2048];
    
    while (receiving_) {
        struct sockaddr_in remoteAddr = {};
        socklen_t addrLen = sizeof(remoteAddr);
        
        ssize_t received = recvfrom(rtpSocket_, buffer, sizeof(buffer), 0,
                                    (struct sockaddr*)&remoteAddr, &addrLen);
        
        if (received > 12) {
            RtpPacket packet;
            if (packet.deserialize(buffer, received)) {
                jitterBuffer_.push(packet);
                
                // Доставка пакетов в порядке
                while (auto p = jitterBuffer_.pop()) {
                    stats_.packetsReceived++;
                    if (receiveCallback_) {
                        receiveCallback_(*p);
                    }
                }
            }
        }
    }
}
```

**Преимущества:**
- Сортировка пакетов по sequence number
- Обработка потерянных пакетов
- Расчёт джиттера в реальном времени
- Защита от переполнения

---

### Рекомендация #13: PCAP экспорт

**Приоритет:** 🟢 Полезный  
**Сложность:** Средняя

**Решение:**
```cpp
// src/utils/pcap_writer.hpp
#pragma once

#include <string>
#include <cstdint>
#include <pcap.h>

namespace h323p {

class PcapWriter {
public:
    explicit PcapWriter(const std::string& filename);
    ~PcapWriter();
    
    // Запись пакета
    void writePacket(const uint8_t* data, size_t size,
                     const std::string& srcIp, int srcPort,
                     const std::string& dstIp, int dstPort,
                     bool isOutgoing);
    
    // Закрытие файла
    void close();
    
private:
    pcap_t* handle_ = nullptr;
    pcap_dumper_t* dumper_ = nullptr;
    bool isLinkLayer_ = false;  // false = raw IP
};

} // namespace h323p
```

**Реализация:**
```cpp
// src/utils/pcap_writer.cpp
#include "pcap_writer.hpp"
#include <cstring>

namespace h323p {

PcapWriter::PcapWriter(const std::string& filename) {
    // Создаём fake pcap_t для dump
    handle_ = pcap_open_dead(DLT_EN10MB, 65535);
    if (!handle_) {
        throw std::runtime_error("Failed to create pcap handle");
    }
    
    dumper_ = pcap_dump_open(handle_, filename.c_str());
    if (!dumper_) {
        pcap_close(handle_);
        throw std::runtime_error("Failed to open pcap file");
    }
}

PcapWriter::~PcapWriter() {
    close();
}

void PcapWriter::writePacket(const uint8_t* data, size_t size,
                              const std::string& srcIp, int srcPort,
                              const std::string& dstIp, int dstPort,
                              bool isOutgoing) {
    if (!dumper_) return;
    
    // Создаём fake Ethernet + IP + UDP заголовки
    struct FakeHeader {
        uint8_t ether_dhost[6] = {0};
        uint8_t ether_shost[6] = {0};
        uint16_t ether_type = htons(0x0800);  // IP
        uint8_t ip_version = 0x45;
        uint8_t ip_tos = 0;
        uint16_t ip_len;
        uint16_t ip_id = 0;
        uint16_t ip_off = 0;
        uint8_t ip_ttl = 64;
        uint8_t ip_proto = 17;  // UDP
        uint16_t ip_check = 0;
        uint32_t ip_src;
        uint32_t ip_dst;
        uint16_t udp_sport;
        uint16_t udp_dport;
        uint16_t udp_len;
        uint16_t udp_check = 0;
    } __attribute__((packed));
    
    FakeHeader hdr;
    hdr.ip_len = htons(sizeof(uint32_t) * 2 + sizeof(uint16_t) * 3 + size);
    hdr.ip_src = inet_addr(srcIp.c_str());
    hdr.ip_dst = inet_addr(dstIp.c_str());
    hdr.udp_sport = htons(srcPort);
    hdr.udp_dport = htons(dstPort);
    hdr.udp_len = htons(sizeof(uint16_t) * 3 + size);
    
    // Собираем пакет
    std::vector<uint8_t> packet(sizeof(FakeHeader) + size);
    memcpy(packet.data(), &hdr, sizeof(FakeHeader));
    memcpy(packet.data() + sizeof(FakeHeader), data, size);
    
    // Запись в pcap
    struct pcap_pkthdr pktHdr;
    pktHdr.caplen = packet.size();
    pktHdr.len = packet.size();
    gettimeofday(&pktHdr.ts, nullptr);
    
    pcap_dump(reinterpret_cast<u_char*>(dumper_), &pktHdr, packet.data());
}

void PcapWriter::close() {
    if (dumper_) {
        pcap_dump_close(dumper_);
        dumper_ = nullptr;
    }
    if (handle_) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

} // namespace h323p
```

**CLI опция:**
```cpp
// src/cli/cli_parser.cpp
app.add_option("--pcap", config.pcapFile, "Файл для захвата трафика");
app.add_option("--pcap-filter", config.pcapFilter, "Фильтр пакетов (h225,h245,rtp)");

// В h323_wrapper.cpp
if (!config.pcapFile.empty()) {
    pcapWriter_ = std::make_unique<PcapWriter>(config.pcapFile);
}
```

**Примеры использования:**
```bash
# Захват всего трафика
./h323p call 192.168.1.100 --pcap capture.pcap

# Только сигнализация
./h323p call 192.168.1.100 --pcap signaling.pcap --pcap-filter h225,h245

# Только медиа
./h323p call 192.168.1.100 --pcap media.pcap --pcap-filter rtp,rtcp
```

**Просмотр в Wireshark:**
```bash
wireshark capture.pcap
```

---

## 🧪 Проверка результатов

### Сборка

```bash
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)
```

### Тесты

```bash
# Тесты RTP
ctest -R "RtpTest" --output-on-failure

# Тесты кодеков
ctest -R "CodecTest" --output-on-failure

# Тесты H.245
ctest -R "H245Test" --output-on-failure
```

### Интеграционный тест

```bash
# Запуск с аудио
./h323p call 192.168.1.100 --audio-codec G.711

# С генератором тона
./h323p call 192.168.1.100 --tone-frequency 1000

# С записью
./h323p call 192.168.1.100 --record output.wav
```

### Пример вывода

```
2026-03-16 10:00:00.123 [INFO] Call established: h323p-abc-1
2026-03-16 10:00:00.124 [INFO] Starting H.245 session
2026-03-16 10:00:00.125 [INFO] Sending Master/Slave Determination
2026-03-16 10:00:00.226 [INFO] Result: we are MASTER
2026-03-16 10:00:00.227 [INFO] Sending Terminal Capability Set
2026-03-16 10:00:00.328 [INFO] Capabilities exchanged
2026-03-16 10:00:00.329 [INFO] Opening logical channel 1: audio G.711 μ-law
2026-03-16 10:00:00.330 [INFO] RTP session: 0.0.0.0:10000 -> 192.168.1.100:20000
2026-03-16 10:00:00.331 [INFO] Media started
2026-03-16 10:00:00.332 [INFO] Sending audio (G.711, 64 kbps)
...
2026-03-16 10:00:30.333 [INFO] Call statistics:
  Duration: 30.0 sec
  RTP packets sent: 1500
  RTP packets received: 1498
  Jitter: 2.5 ms
```

---

## 📚 Ссылки

- **H.245 стандарт:** ITU-T Recommendation H.245
- **RTP RFC 3550:** https://datatracker.ietf.org/doc/html/rfc3550
- **G.711 стандарт:** ITU-T Recommendation G.711
- **H323Plus Media:** https://www.h323plus.org/api/

---

*Версия документа: 0.1.0*  
*Последнее обновление: 2026-03-16*
