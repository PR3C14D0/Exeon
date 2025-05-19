#include "Subsystem/Discord.h"
#define DISCORDPP_IMPLEMENTATION
#include <Discord/discordpp.h>

Discord* Discord::m_instance;
std::atomic<bool> running = true;

Discord::Discord() {
    this->m_appId = 0;
}

void Discord::ClientThread() {
    auto client = std::make_shared<discordpp::Client>();
    spdlog::info("Discord Social SDK: Running on a detached thread");

    client->AddLogCallback([](std::string message, auto severity) {
        std::string cleanMessage = message;
        cleanMessage.erase(std::remove(cleanMessage.begin(), cleanMessage.end(), '\n'), cleanMessage.end());
        spdlog::debug("Discord Social SDK: {0}", cleanMessage);
    }, discordpp::LoggingSeverity::Info);

    client->SetStatusChangedCallback([&client](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) {
        spdlog::debug("Discord Social SDK: Status changed to {0}", discordpp::Client::StatusToString(status));

        if (status == discordpp::Client::Status::Ready) {
            spdlog::debug("Discord Social SDK: Client ready!");

            discordpp::Activity activity;
            activity.SetType(discordpp::ActivityTypes::Playing);
            activity.SetState("Exeon Game Engine v0.1");
            activity.SetDetails("Editing Fiwa");

            // Update rich presence
            client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
                if(result.Successful()) {
                    spdlog::debug("Discord Social SDK: Rich Presence updated");
                } else {
                    spdlog::error("Discord Social SDK: Error updating Rich Presence");
                }
            });
        } else if (error != discordpp::Client::Error::None) {
            spdlog::debug("Discord Social SDK: Client disconnected!");
        }
    });

    auto codeVerifier = client->CreateAuthorizationCodeVerifier();

    discordpp::AuthorizationArgs args{};
    args.SetClientId(this->m_appId);
    args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
    args.SetCodeChallenge(codeVerifier.Challenge());

    client->Authorize(args, [client, codeVerifier, this](auto result, auto code, auto redirectUri) {
      if (!result.Successful()) {
          spdlog::error("Discord Social SDK: Authorization failed!");
      } else {
          spdlog::debug("Discord Social SDK: Authorization successful!");

        // Exchange auth code for access token
          client->GetToken(this->m_appId, code, codeVerifier.Verifier(), redirectUri,
            [client](discordpp::ClientResult result,
            std::string accessToken,
            std::string refreshToken,
            discordpp::AuthorizationTokenType tokenType,
            int32_t expiresIn,
            std::string scope) {
                spdlog::debug("Discord Social SDK: Establishing connection");
                client->UpdateToken(discordpp::AuthorizationTokenType::Bearer,  accessToken, [client](discordpp::ClientResult result) {
                    if(result.Successful()) {
                        spdlog::info("Discord Social SDK: Token updated");
                        client->Connect();
                    }
                });

          });
      }
    });

    while (running) {
        discordpp::RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


void Discord::Init() {
    this->m_appId = 1374036377322258544;
    std::thread discordThread(&Discord::ClientThread, this);
    discordThread.detach();
}

Discord* Discord::GetInstance() {
    if (Discord::m_instance == nullptr)
        Discord::m_instance = new Discord();
    return Discord::m_instance;
}