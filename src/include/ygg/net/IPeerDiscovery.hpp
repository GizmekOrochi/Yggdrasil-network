#pragma once

/**
 * @file IPeerDiscovery.hpp
 * @brief Port de decouverte sans implication de confiance.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include <functional>
#include <optional>

namespace ygg::net {

/**
 * @brief Adresse reseau potentielle signalee par decouverte.
 * @details claimed_identity est une affirmation non fiable; seule la preuve TLS + truststore
 * etablit un PeerId.
 * @par Responsabilite du fichier Port d enumeration d adresses candidates.
 * @par Regles importantes Le champ claimed_identity n est jamais une preuve. Aucune connexion ne
 * devient autorisee sans preuve cryptographique independante.

 * @par Contrat complementaire initial
 * Untrusted endpoint advertised by discovery.
 */
struct PeerCandidate {
    /**
     * @brief Adresse candidate qui doit etre validee lors du dial.
     */
    Endpoint endpoint{};
    /**
     * @brief Champ claimed_identity de PeerCandidate : valeur interne conforme aux invariants de ce
     * type.
     */
    std::optional<PeerId> claimed_identity{};
};

/**
 * @brief Callback recevant seulement des adresses candidates non authentifiees.
 */
using DiscoveryCallback = std::function<void(PeerCandidate)>;

/**
 * @brief Composant IPeerDiscovery du module net.
 * @details Signale des adresses potentielles trouvees par mDNS ou configurees manuellement.
 * @par Responsabilite du fichier Port d enumeration d adresses candidates.
 * @par Regles importantes Le champ claimed_identity n est jamais une preuve. Aucune connexion ne
 * devient autorisee sans preuve cryptographique independante.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Candidate enumeration; never grants trust.
 */
class IPeerDiscovery {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IPeerDiscovery() = default;
    /**
     * @brief Demarre l emission de candidats de connexion via callback; aucune autorisation n en
     * decoule.
     * @param callback Recepteur des candidats emis par decouverte.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<void> start(DiscoveryCallback callback) = 0;
    /**
     * @brief Arrete l emission de candidats et libere les ressources de decouverte.
     */
    virtual void stop() noexcept = 0;
};

} // namespace ygg::net
