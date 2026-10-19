#pragma once

/**
 * @file Messages.hpp
 * @brief DTO applicatifs YGG/1 independants du stockage.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace ygg::protocol {

/**
 * @brief Structure de donnees Hello utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Application-level introduction sent only after mutual TLS approval.
 */
struct Hello {
    /**
     * @brief Identite annoncee qui doit correspondre au pair authentifie du canal.
     */
    PeerId sender{};
    /**
     * @brief Libelle visible sans valeur d authentification.
     */
    std::string display_name{};
    /**
     * @brief Version mineure negociable sans rupture majeure.
     */
    std::uint32_t minor_version{};
};

/**
 * @brief Structure de donnees Welcome utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Compatible handshake response.
 */
struct Welcome {
    /**
     * @brief Identite annoncee qui doit correspondre au pair authentifie du canal.
     */
    PeerId sender{};
    /**
     * @brief Version mineure negociable sans rupture majeure.
     */
    std::uint32_t minor_version{};
};

/**
 * @brief Sonde de connectivite avec identifiant de correlation.
 * @details Le nonce est un nombre 64 bits arbitraire utilise uniquement pour associer Pong au Ping.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Round-trip time request; nonce prevents reply confusion.
 */
struct Ping {
    /**
     * @brief Nonce de segment AEAD ou correlateur Ping/Pong selon le type; jamais reutilise dans un
     * contexte cryptographique.
     */
    std::uint64_t nonce{};
};

/**
 * @brief Reponse a la sonde de connectivite.
 * @details Recopie exactement le nonce du Ping recu afin d eviter les confusions.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.
 */
struct Pong {
    /**
     * @brief Nonce de segment AEAD ou correlateur Ping/Pong selon le type; jamais reutilise dans un
     * contexte cryptographique.
     */
    std::uint64_t nonce{};
};

/**
 * @brief Structure de donnees PeerAdvertisement utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Controlled disclosure of known peer addresses, not authorization.
 */
struct PeerAdvertisement {
    /**
     * @brief Identifiant du pair concerne, jamais substitut aux preuves d identite.
     */
    PeerId peer{};
    /**
     * @brief Adresses annoncees d un pair; liste non authentifiante.
     */
    std::vector<Endpoint> addresses{};
};

/**
 * @brief Structure de donnees PeerList utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.
 */
struct PeerList {
    /**
     * @brief Liste de pairs annonces, a traiter comme candidates non fiables.
     */
    std::vector<PeerAdvertisement> peers{};
};

/**
 * @brief Structure de donnees DataMessage utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Namespaced binary application envelope.
 */
struct DataMessage {
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
 * @brief Structure de donnees ErrorMessage utilisee par protocol.
 * @details Definit HELLO/WELCOME, PING/PONG, annonces de pairs, transport de payload et erreur.
 * @par Responsabilite du fichier Objets de messages applicatifs neutres.
 * @par Regles importantes Ces structures ne sont pas un format filaire en elles-memes : preciser la
 * serialisation stable et les regles de compatibilite.

 * @par Contrat complementaire initial
 * Protocol-level error without secret diagnostic details.
 */
struct ErrorMessage {
    /**
     * @brief Categorie d erreur stable et independante des exceptions des bibliotheques externes.
     */
    ErrorCode code{ErrorCode::internal_error};
    /**
     * @brief Résumé d erreur nettoye des informations sensibles.
     */
    std::string summary{};
};

} // namespace ygg::protocol
