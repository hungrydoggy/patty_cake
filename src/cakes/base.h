#ifndef PATTY_CAKE_CAKES_BASE_H_
#define PATTY_CAKE_CAKES_BASE_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>



namespace patty_cake {


struct PattyClientInfo;
struct PattyCakePiece;


class PattyCake {
public: // inner types
  enum class Type {
    WEB_SOCKET,
    WEB_RTC,
  };

  enum class State {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    FAILED,

    LISTENING,
  };

  using OnMessageFunc = std::function<void(PattyCake* cake, PattyCakePiece const& piece)>;
  using OnStateChangeFunc = std::function<void(PattyCake* cake, State state)>;
  using OnLocalSdpFunc = std::function<void(std::string)>;
  using OnLocalIceFunc = std::function<void(std::string)>;

  struct ConnectConfig {
    std::string       name;
    Type              type;
    std::string       url;
    OnMessageFunc     on_message_func;
    OnStateChangeFunc on_state_change_func;

    // WebRTC
    bool           ordered         = true;
    int            max_retransmits = -1;    // -1 == ∞
    OnLocalSdpFunc on_local_sdp_func;
    OnLocalIceFunc on_local_ice_func;
  };

  struct ListenConfig {
    std::string       name;
    Type              type;
    std::string       host = "0.0.0.0";
    int               port = 9991;
    OnMessageFunc     on_message_func;
    OnStateChangeFunc on_state_change_func;

    // WebRTC
    OnLocalSdpFunc on_local_sdp_func;
    OnLocalIceFunc on_local_ice_func;
  };


public: // static methods
  static std::shared_ptr<PattyCake> connect (ConnectConfig const& cnf);

  static std::shared_ptr<PattyCake> listen (ListenConfig const& cnf);


public: // getter/setter
  virtual Type type () const = 0;

  inline State state () const { return state_; }

  inline void on_message_func (OnMessageFunc f) { on_message_func_ = f; }

  inline void on_state_change_func (OnStateChangeFunc f) { on_state_change_func_ = f; }

  inline std::unordered_map<std::string, std::shared_ptr<PattyClientInfo>> const&
  client_info_map () const { return client_info_map_; }


public: // methods
  virtual void disconnect () = 0;

  virtual bool send (std::vector<uint8_t> const& data) = 0;

  virtual bool send (std::string const& id, std::vector<uint8_t> const& data) = 0;

  virtual void poll () = 0;


protected: // getter/setter
  void state (State v);

  inline OnMessageFunc on_message_func () const { return on_message_func_; }

  inline OnStateChangeFunc on_state_change_func () const { return on_state_change_func_; }


protected: // methods
  PattyCake ();

  void _addClientInfo (std::shared_ptr<PattyClientInfo> const& info);

  void _removeClientInfo (std::string const& id);

  std::shared_ptr<PattyClientInfo> _findClientInfo (std::string const& id) const;


private: // vars
  State state_;
  OnMessageFunc on_message_func_;
  OnStateChangeFunc on_state_change_func_;

  std::unordered_map<std::string, std::shared_ptr<PattyClientInfo>> client_info_map_;
};



struct PattyClientInfo {
  std::string id;

  PattyClientInfo (std::string const& id);

  virtual bool is_alive () const = 0;
};



}
#endif  // ifndef PATTY_CAKE_CAKES_BASE_H_
