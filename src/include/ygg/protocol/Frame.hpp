#pragma once

/**
 * @file Frame.hpp
 * @brief En-tete fixe YGG/1 et trames.
 */

#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <array>
#include <cstdint>

namespace ygg::protocol {

/**
 * @brief Liste des etats/codes stables de FrameType.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier En-tete binaire YGG/1 et trames en memoire.
 * @par Regles importantes Les champs structures ne doivent jamais etre envoyes par memcpy;
 * convertir explicitement les entiers en big-endian.

 * @par Contrat complementaire initial
 * Explicit YGG/1 wire message category.
 */
enum class FrameType : std::uint8_t {
    /**
     * @brief Presentation apres TLS.
     */
    hello=0x01,
    /**
     * @brief Accord sur la version YGG/1.
     */
    welcome=0x02,
    /**
     * @brief Sonde avec nonce.
     */
    ping=0x03,
    /**
     * @brief Reponse a la sonde.
     */
    pong=0x04,
    /**
     * @brief Liste d adresses candidates de pairs.
     */
    peer_list=0x05,
    /**
     * @brief Conteneur de payload de service.
     */
    data=0x06,
    /**
     * @brief Annonce de fermeture volontaire.
     */
    goodbye=0x07,
    /**
     * @brief Erreur filaire non sensible.
     */
    error=0x08
};

/**
 * @brief En-tete filaire de 20 octets en ordre hote.
 * @details serialize/parse doivent manipuler les champs individuellement, en big-endian et sans
 * padding C++.
 * @par Responsabilite du fichier En-tete binaire YGG/1 et trames en memoire.
 * @par Regles importantes Les champs structures ne doivent jamais etre envoyes par memcpy;
 * convertir explicitement les entiers en big-endian.

 * @par Contrat complementaire initial
 * YGG/1 fixed-size 20-byte header; wire integers are big-endian. Fields are represented in host
 * order in memory.
 */
struct FrameHeader {
    /**
     * @brief Signature ASCII fixe YGG1 pour detecter les protocoles incompatibles.
     */
    std::array<char,4> magic{{'Y','G','G','1'}};
    /**
     * @brief Version majeure du protocole; incompatibilite a refuser.
     */
    std::uint8_t major{1};
    /**
     * @brief Champ type de FrameHeader : valeur interne conforme aux invariants de ce type.
     */
    FrameType type{FrameType::data};
    /**
     * @brief Bits d options; refuser les bits reserves non connus.
     */
    std::uint16_t flags{};
    /**
     * @brief Correlation d une demande/reponse filaire dans une session.
     */
    std::uint64_t message_id{};
    /**
     * @brief Nombre d octets annonces; doit etre egal au vrai payload.
     */
    std::uint32_t payload_length{};

    /**
     * @brief Controle magic, version, type, flags reserves et longueur du payload avant tout
     * decodage.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Reject unsupported versions, flags and oversized payload lengths.
 */
    [[nodiscard]] Result<void> validate() const;
};

/**
 * @brief Trame YGG/1 complete avec payload possede.
 * @details Le champ header.payload_length doit correspondre aux octets reellement presents et
 * respecter le plafond de framing.
 * @par Responsabilite du fichier En-tete binaire YGG/1 et trames en memoire.
 * @par Regles importantes Les champs structures ne doivent jamais etre envoyes par memcpy;
 * convertir explicitement les entiers en big-endian.

 * @par Contrat complementaire initial
 * In-memory frame with owned payload.
 */
struct Frame {
    /**
     * @brief Champ header de Frame : valeur interne conforme aux invariants de ce type.
     */
    FrameHeader header{};
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    Bytes payload{};
};

} // namespace ygg::protocol
