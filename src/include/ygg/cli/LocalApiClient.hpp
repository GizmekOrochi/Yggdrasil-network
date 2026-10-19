#pragma once

/**
 * @file LocalApiClient.hpp
 * @brief Client CLI au dessus du socket Unix local.
 */

#include "ygg/daemon/LocalApi.hpp"
#include "ygg/core/Result.hpp"

namespace ygg::cli {

/**
 * @brief Composant LocalApiClient du module cli.
 * @details Encode les commandes LocalCommand, contacte exclusivement le socket Unix de yggd et
 * decode LocalResponse.
 * @par Responsabilite du fichier Client de controle local employe par yggctl.
 * @par Regles importantes Ne jamais ouvrir une connexion P2P depuis la CLI. Valider la provenance
 * du serveur local, limiter les tailles et traiter proprement sa disparition.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Thin yggctl client: never connects directly to remote peers.
 */
class LocalApiClient final {
public:
    /**
     * @brief Memorise le chemin du socket Unix du demon; ne declenche aucune requete reseau.
     * @param daemon_socket Chemin du socket Unix d administration de yggd.
     */
    explicit LocalApiClient(Path daemon_socket);
    /**
     * @brief Serialise la commande locale, transmet une seule requete au demon et retourne la
     * reponse correspondante; deadline et encodage local a definir.
     * @param command Commande locale a executer.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<daemon::LocalResponse> request(daemon::LocalCommand command);
private:
    /**
     * @brief Etat/dependance interne daemon_socket_ : garder la validite, la synchronisation et la
     * duree de vie definies par LocalApiClient.
     */
    Path daemon_socket_{};
};
} // namespace ygg::cli
