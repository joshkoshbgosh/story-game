#ifndef STORY_H
#define STORY_H
#include "json.hpp"
#include <optional>

using json = nlohmann::json;

namespace Story {
  namespace Shared {
    enum GameStatus {
      PENDING,
      ACTIVE
    };

    struct User {
      std::string id;
    };

    struct Game {
      std::string id;
      GameStatus status;
      std::vector<User> users;
    };


    // You need to handle GameStatus manually since it's an enum
    std::string gameStatusToString(GameStatus status); 

    GameStatus stringToGameStatus(const std::string& status);

    // macros to support serialization
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(User, id)

    NLOHMANN_JSON_SERIALIZE_ENUM(GameStatus, {
        {PENDING, "PENDING"},
        {ACTIVE, "ACTIVE"},
    })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Game, id, status, users)




    // any request the client makes will have mostly shared state, use RequestState to reduce duplication
    enum class RequestStatus {
      LOADING,
      ERROR,
      SUCCESS,
    };

    template<typename RequestBodyType, typename ResponseDataType, typename ErrorType>
    struct RequestState {
      RequestStatus status;
      std::optional<RequestBodyType> body;
      std::optional<ResponseDataType> response;
      std::optional<ErrorType> error;
    };
    
    struct NullType {};
  
    struct CreateGameResponseDataType {
      Story::Shared::Game game;
    };
    
    struct JoinGameResponseDataType {
      Story::Shared::Game game;
    };
  }

  namespace Client {
    struct ClientState {
      std::string username;
      std::optional<Story::Shared::Game> game;
      std::optional<
        Story::Shared::RequestState<
          Story::Shared::NullType, 
          Story::Shared::CreateGameResponseDataType, 
          std::string
        >
      > createGameRequestState;
      std::optional<
        Story::Shared::RequestState<
          std::string, 
          Story::Shared::JoinGameResponseDataType, 
          std::string
        >
      > joinGameRequestState;      
    };
    // define possible actions and map to the expected payloads
    enum class ActionType {
      SET_USERNAME,

      SET_CREATE_GAME_LOADING,
      SET_CREATE_GAME_ERROR,
      SET_CREATE_GAME_COMPLETE,

      SET_JOIN_GAME_LOADING,
      SET_JOIN_GAME_ERROR,
      SET_JOIN_GAME_COMPLETE,
    };

    template<ActionType> struct ActionPayloadMap;

    template<ActionType T>
    struct Action {
        typename ActionPayloadMap<T>::type payload;
    };
    // use this as the payload type for actions that don't need a payload 
    struct NoPayload {};

    // Template specializations for each action type

    // SET_USERNAME
    struct SetUsernamePayload {
      std::string username;
    };
    template<>
    struct ActionPayloadMap<ActionType::SET_USERNAME> {
        using type = SetUsernamePayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_USERNAME>& action);

    // SET_CREATE_GAME_LOADING
    template<>
    struct ActionPayloadMap<ActionType::SET_CREATE_GAME_LOADING> {
        using type = NoPayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_CREATE_GAME_LOADING>& action);

    // SET_CREATE_GAME_ERROR
    struct SetCreateGameErrorPayload {
      std::string error;
    };
    template<>
    struct ActionPayloadMap<ActionType::SET_CREATE_GAME_ERROR> {
        using type = SetCreateGameErrorPayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_CREATE_GAME_ERROR>& action);

    // SET_CREATE_GAME_COMPLETE
    struct SetCreateGameCompletePayload {
      Story::Shared::Game game;
    };
    template<>
    struct ActionPayloadMap<ActionType::SET_CREATE_GAME_COMPLETE> {
        using type = SetCreateGameCompletePayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_CREATE_GAME_COMPLETE>& action);

    // SET_JOIN_GAME_LOADING
    template<>
    struct ActionPayloadMap<ActionType::SET_JOIN_GAME_LOADING> {
        using type = NoPayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_JOIN_GAME_LOADING>& action);

    // SET_JOIN_GAME_ERROR
    struct SetJoinGameErrorPayload {
      std::string error;
    };
    template<>
    struct ActionPayloadMap<ActionType::SET_JOIN_GAME_ERROR> {
        using type = SetJoinGameErrorPayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_JOIN_GAME_ERROR>& action);

    // SET_JOIN_GAME_COMPLETE
    struct SetJoinGameCompletePayload {
      Story::Shared::Game game;
    };
    template<>
    struct ActionPayloadMap<ActionType::SET_JOIN_GAME_COMPLETE> {
        using type = SetCreateGameCompletePayload;
    };
    ClientState handleAction(ClientState currentState, const Action<ActionType::SET_JOIN_GAME_COMPLETE>& action);


    
    //ignore, only need to handle serialization of messages between client and server
    /* NLOHMANN_JSON_SERIALIZE_ENUM(ActionType, { */
    /*   {ActionType::SET_USERNAME, "SET_USERNAME"}, */
    /*   {ActionType::SET_GAME, "SET_GAME"}, */
    /* }) */
    /**/
    /* auto payloadToJson = [](const auto& payload) { */
    /*   return json(payload); */
    /* }; */
  }

  namespace Server {
    struct ServerState {
    }; 
  }
}

#endif
