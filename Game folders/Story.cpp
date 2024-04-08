#include "json.hpp"
#include "Story.h"

using json = nlohmann::json;

namespace Story {
  namespace Shared {

  }

  namespace Client {

    
    // define possible actions and map to the expected payloads


    template<ActionType> struct ActionPayloadMap;
    

    // Template specializations for each action type

    // SET_USERNAME
    /* struct SetUsernamePayload { */
    /*   std::string username; */
    /* }; */
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_USERNAME> { */
    /*     using type = SetMyUsernamePayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_USERNAME> action) { */
    /*   return currentState; */
    /* } */

    // SET_CREATE_GAME_LOADING
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_CREATE_GAME_LOADING> { */
    /*     using type = NoPayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_CREATE_GAME_LOADING> action) { */
    /*   return currentState; */
    /* } */

    // SET_CREATE_GAME_ERROR
    /* struct SetCreateGameErrorPayload { */
    /*   std::string error; */
    /* } */
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_CREATE_GAME_ERROR> { */
    /*     using type = SetCreateGameErrorPayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_CREATE_GAME_ERROR> action) { */
    /*   return currentState; */
    /* } */

    // SET_CREATE_GAME_COMPLETE
    /* struct SetCreateGameCompletePayload { */
    /*   Story::Shared::Game game; */
    /* } */
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_CREATE_GAME_COMPLETE> { */
    /*     using type = SetCreateGameErrorPayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_CREATE_GAME_COMPLETE> action) { */
    /*   return currentState; */
    /* } */

    // SET_JOIN_GAME_LOADING
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_JOIN_GAME_LOADING> { */
    /*     using type = NoPayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_JOIN_GAME_LOADING> action) { */
    /*   return currentState; */
    /* } */

    // SET_JOIN_GAME_ERROR
    /* struct SetJoinGameErrorPayload { */
    /*   std::string error; */
    /* } */
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_JOIN_GAME_ERROR> { */
    /*     using type = SetJoinGameErrorPayload; */
    /* }; */
    /* ClientState handleAction(ClientState currentState, Action<ActionType::SET_JOIN_GAME_LOADING> action) { */
    /*   return currentState; */
    /* } */

    // SET_JOIN_GAME_COMPLETE
    /* struct SetCreateGameErrorPayload { */
    /*   std::string error; */
    /* } */
    /* template<> */
    /* struct ActionPayloadMap<ActionType::SET_CREATE_GAME_ERROR> { */
    /*     using type = SetCreateGameErrorPayload; */
    /* }; */


    /* template<ActionType T> */
    /* struct Action { */
    /*     typename ActionPayloadMap<T>::type payload; */
    /* }; */

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
  }
}
