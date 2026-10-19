#pragma once

/**
 * @file MessageRouter.hpp
 * @brief Aiguillage RPC vers les services applicatifs.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <map>
#include <memory>
#include <string>

namespace ygg::protocol {

/**
 * @brief Requete RPC portee par le service YGG/1.
 * @details request_id sert a corréler la reponse et service choisit le handler.
 * @par Responsabilite du fichier Routeur RPC des services applicatifs.
 * @par Regles importantes Un ServiceId inconnu renvoie une erreur explicite; eviter qu un service
 * bloque la boucle d evenements.

 * @par Contrat complementaire initial
 * Dispatched request within an authenticated peer session.
 */
struct Request {
    /**
     * @brief Champ request_id de Request : valeur interne conforme aux invariants de ce type.
     */
    RequestId request_id{};
    /**
     * @brief Nom de service versionne permettant l aiguillage.
     */
    ServiceId service{};
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    Bytes payload{};
};

/**
 * @brief Resultat RPC avec identifiant de correlation.
 * @details Le tracker n accepte la reponse que si request_id correspond a une attente ouverte.
 * @par Responsabilite du fichier Routeur RPC des services applicatifs.
 * @par Regles importantes Un ServiceId inconnu renvoie une erreur explicite; eviter qu un service
 * bloque la boucle d evenements.
 */
struct Response {
    /**
     * @brief Champ request_id de Response : valeur interne conforme aux invariants de ce type.
     */
    RequestId request_id{};
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    Bytes payload{};
};

/**
 * @brief Composant IMessageHandler du module protocol.
 * @details Oriente une requete vers un gestionnaire en fonction du ServiceId apres authentification
 * reseau.
 * @par Responsabilite du fichier Routeur RPC des services applicatifs.
 * @par Regles importantes Un ServiceId inconnu renvoie une erreur explicite; eviter qu un service
 * bloque la boucle d evenements.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Service-side RPC contract; no transport or storage coupling.
 */
class IMessageHandler {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IMessageHandler() = default;
    /**
     * @brief Traite une requete deja routee, sous l identite du pair authentifie; retourne une
     * reponse correlee.
     * @param sender PeerId authentifie qui a soumis la requete.
     * @param request Requete a dispatcher ou enregistrer.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    virtual Async<Response> handle(PeerId sender, Request request) = 0;
};

/**
 * @brief Composant MessageRouter du module protocol.
 * @details Oriente une requete vers un gestionnaire en fonction du ServiceId apres authentification
 * reseau.
 * @par Responsabilite du fichier Routeur RPC des services applicatifs.
 * @par Regles importantes Un ServiceId inconnu renvoie une erreur explicite; eviter qu un service
 * bloque la boucle d evenements.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Register handlers for namespaced protocol services.
 */
class MessageRouter final {
public:
    /**
     * @brief Enregistre un handler sous un ServiceId valide et refuse les collisions non
     * explicites.
     * @param service Identifiant de service, avec espace de noms/version.
     * @param handler Gestionnaire applicatif partage, responsable du traitement RPC.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> add(ServiceId service, std::shared_ptr<IMessageHandler> handler);
    /**
     * @brief Cherche le handler et transmet la requete avec le vrai PeerId du demandeur.
     * @param sender PeerId authentifie qui a soumis la requete.
     * @param request Requete a dispatcher ou enregistrer.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<Response> route(PeerId sender, Request request);
    /**
     * @brief Retire l inscription d un service sans annuler les appels deja en cours sans politique
     * expresse.
     * @param service Identifiant de service, avec espace de noms/version.
     */
    void remove(const ServiceId& service);
private:
    /**
     * @brief Associe chaque nom de service au handler possede par shared_ptr.
     */
    std::map<std::string, std::shared_ptr<IMessageHandler>> handlers_{};
};
} // namespace ygg::protocol
