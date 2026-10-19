#pragma once

/**
 * @file MdnsDiscovery.hpp
 * @brief Decouverte mDNS de candidats sur le LAN.
 */

#include "ygg/net/IPeerDiscovery.hpp"
#include <string>

namespace ygg::net {

/**
 * @brief Composant MdnsDiscovery du module net.
 * @details Annonce le service _ygg._tcp.local et rapporte les pairs candidats.
 * @par Responsabilite du fichier Decouverte automatique sur le LAN.
 * @par Regles importantes Pas de routage Internet via mDNS; annonces non fiables et simples indices
 * de connexion.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * LAN-only mDNS adapter for _ygg._tcp.local addresses.
 */
class MdnsDiscovery final : public IPeerDiscovery {
public:
    /**
     * @brief Prepare l annonce du service local et son nom descriptif, sans demarrer mDNS.
     * @param local_address Adresse locale que mDNS doit annoncer.
     * @param advertised_name Nom d annonce mDNS purement descriptif.
     */
    MdnsDiscovery(Endpoint local_address, std::string advertised_name);
    /**
     * @brief Annonce le service et notifie les decouvertes mDNS sous forme de candidats non
     * fiables.
     * @param callback Recepteur des candidats emis par decouverte.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @warning La decouverte n accorde aucun droit : le certificat du candidat devra etre verifie.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> start(DiscoveryCallback callback) override;
    /**
     * @brief Retire l annonce locale et arrete proprement la reception mDNS.
     */
    void stop() noexcept override;
private:
    /**
     * @brief Adresse de service annoncee sur le LAN.
     */
    Endpoint local_address_{};
    /**
     * @brief Nom d annonce non authentifie, purement cosmetique.
     */
    std::string advertised_name_{};
};

} // namespace ygg::net
