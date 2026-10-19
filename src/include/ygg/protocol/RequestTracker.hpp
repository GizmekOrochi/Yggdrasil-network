#pragma once

/**
 * @file RequestTracker.hpp
 * @brief Suivi borne des requetes RPC.
 */

#include "ygg/protocol/MessageRouter.hpp"
#include <map>
#include <array>
#include <cstddef>

namespace ygg::protocol {

/**
 * @brief Structure de donnees PendingRequest utilisee par protocol.
 * @details Garde une capacite borne, associe RequestId a expiration et gere completion et
 * expiration.
 * @par Responsabilite du fichier Suivi des requetes RPC en attente.
 * @par Regles importantes Ne pas accepter deux requetes identiques en cours; nettoyer immediatement
 * les requetes resolues ou expirees.

 * @par Contrat complementaire initial
 * Pending request outcome correlated by request_id.
 */
struct PendingRequest {
    /**
     * @brief Identifiant opaque et stable de cette entite.
     */
    RequestId id{};
    /**
     * @brief Instant limite monotone d une requete.
     */
    MonotonicTime deadline{};
};

/**
 * @brief Composant RequestTracker du module protocol.
 * @details Garde une capacite borne, associe RequestId a expiration et gere completion et
 * expiration.
 * @par Responsabilite du fichier Suivi des requetes RPC en attente.
 * @par Regles importantes Ne pas accepter deux requetes identiques en cours; nettoyer immediatement
 * les requetes resolues ou expirees.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Track request expiry without unbounded promise growth.
 */
class RequestTracker final {
public:
    /**
     * @brief Definit le maximum de requetes asynchrones simultanement attendues.
     * @param max_pending Nombre limite de requetes pouvant attendre une reponse.
     */
    explicit RequestTracker(std::size_t max_pending);
    /**
     * @brief Enregistre la requete et son expiration; refuse doublons et capacite depassee.
     * @param request Requete a dispatcher ou enregistrer.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> begin(PendingRequest request);
    /**
     * @brief Associe une reponse a une requete en cours et libere l entree correspondante.
     * @param response Reponse dont l identifiant est correle a une attente.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> complete(const Response& response);
    /**
     * @brief Marque l attente terminee sur timeout et libere son emplacement.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> expire(RequestId id);
    /**
     * @brief Renvoie le nombre instantane d attentes toujours ouvertes.
     * @return Nombre actuel d elements suivis.
     */
    [[nodiscard]] std::size_t pending_count() const noexcept;
private:
    /**
     * @brief Capacite totale du suivi des requetes simultanées.
     */
    std::size_t max_pending_{};
    /**
     * @brief Map des requetes en attente et de leurs deadlines monotones.
     */
    std::map<std::array<std::byte,32>, PendingRequest> pending_{};
};
} // namespace ygg::protocol
