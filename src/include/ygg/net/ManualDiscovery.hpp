#pragma once

/**
 * @file ManualDiscovery.hpp
 * @brief Saisie manuelle d adresses candidates.
 */

#include "ygg/net/IPeerDiscovery.hpp"
#include <vector>

namespace ygg::net {

/**
 * @brief Composant ManualDiscovery du module net.
 * @details Fournit une option robuste lorsque mDNS est bloque ou lorsque les machines sont dans
 * plusieurs sous-reseaux.
 * @par Responsabilite du fichier Decouverte par liste statique.
 * @par Regles importantes Valider port et hote, eviter les adresses dupliquees; ne pas supposer qu
 * une adresse restera stable.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Static peer address candidates for routed LANs without mDNS.
 */
class ManualDiscovery final : public IPeerDiscovery {
public:
    /**
     * @brief Ajoute une adresse candidate apres validation syntactique, sans l authentifier.
     * @param endpoint Adresse candidate, non une identite de confiance.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> add(Endpoint endpoint);
    /**
     * @brief Signale les endpoints connus et rend possible la decouverte tardive selon adaptateur.
     * @param callback Recepteur des candidats emis par decouverte.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> start(DiscoveryCallback callback) override;
    /**
     * @brief Suspend les notifications de decouverte manuelle sans effacer les adresses
     * configurees.
     */
    void stop() noexcept override;
private:
    /**
     * @brief Adresses de pair configurees manuellement.
     */
    std::vector<Endpoint> addresses_{};
};

} // namespace ygg::net
