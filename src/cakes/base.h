#ifndef PATTY_CAKE_CAKES_BASE_H_
#define PATTY_CAKE_CAKES_BASE_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <vector>



namespace patty_cake {


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

  using OnMessageFunc = std::function<void(PattyCakePiece const& piece)>;
  using OnStateChangeFunc = std::function<void(State state)>;

  struct ConnectConfig {
    Type              type;
    std::string       url             = "";
    bool              ordered         = true;  // WebRTC
    int               max_retransmits = -1;    // WebRTC. -1 == ∞
    OnMessageFunc     on_message_func;
    OnStateChangeFunc on_state_change_func;
  };

  struct ListenConfig {
    Type              type;
    std::string       host = "0.0.0.0";
    int               port = 8008;
    OnMessageFunc     on_message_func;
    OnStateChangeFunc on_state_change_func;
  };


public: // static methods
  static std::shared_ptr<PattyCake> connect (ConnectConfig const& cnf);

  static std::shared_ptr<PattyCake> listen (ListenConfig const& cnf);


public: // getter/setter
  virtual Type type () const = 0;

  inline State state () const { return state_; }

  inline void on_message_func (OnMessageFunc f) { on_message_func_ = f; }

  inline void on_state_change_func (OnStateChangeFunc f) { on_state_change_func_ = f; }


public: // methods
  virtual void disconnect () = 0;

  virtual bool send (std::vector<uint8_t> const& data) = 0;

  virtual void poll () = 0;


protected: // getter/setter
  void state (State v);

  inline OnMessageFunc on_message_func () const { return on_message_func_; }

  inline OnStateChangeFunc on_state_change_func () const { return on_state_change_func_; }


protected: // methods
  PattyCake ();


private: // vars
  State state_;
  OnMessageFunc on_message_func_;
  OnStateChangeFunc on_state_change_func_;
};


}
#endif  // ifndef PATTY_CAKE_CAKES_BASE_H_
