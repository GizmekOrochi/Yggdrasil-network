#pragma once

/**
 * @file FrameCodec.hpp
 * @brief Encodeur et decodeur incrementaux des trames YGG/1.
 */

#include "ygg/protocol/Frame.hpp"
#include <vector>

namespace ygg::protocol {
    
/**
 * @brief Parseur incrementale resistant aux entrees malformees.
 * @details TCP ne conserve pas les frontieres de messages; une lecture peut contenir une fraction
 * ou plusieurs trames.
 * @par Responsabilite du fichier Encodeur/decodeur incrementaux YGG/1.
 * @par Regles importantes Valider tailles et version avant allocations; borner le buffer accumule
 * et rejeter les entrees malformees.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Incremental, bounded wire-frame encoder and decoder. Supports split and coalesced TCP reads,
 * rejects oversized frames before allocation, and consumes only fully decoded frames.
 */
class FrameCodec final {
public:
    /**
     * @brief Fixe un plafond de payload independent de la taille des lectures brutes.
     * @param max_payload Taille maximale admise pour le payload d une trame.
     */
    explicit FrameCodec(std::size_t max_payload = Limits::max_control_bytes);
    /**
     * @brief Controle la trame et encode les 20 octets d en-tete suivi du payload en big-endian.
     * @param frame Trame YGG/1 dont les donnees sont possedees.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @pre Le champ payload_length doit correspondre exactement a payload.size() ou etre recalcule
     * selon la convention definie.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Encode a complete frame with network byte order.
 */
    [[nodiscard]] Result<Bytes> encode(const Frame& frame) const;
    /**
     * @brief Ajoute le chunk entrant au buffer borne et extrait zero, une ou plusieurs trames
     * completes.
     * @param bytes Vue des octets entrants dont la vie est bornee a cet appel.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @post Un appel peut produire zero, une ou plusieurs trames; les octets incomplets restent
     * caches.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Add received data; may produce zero, one or multiple frames.
 */
    [[nodiscard]] Result<std::vector<Frame>> feed(ByteView bytes);
    /**
     * @brief Supprime uniquement l etat de parsing partiel, sans modifier l etat du transport.

 * @par Contrat complementaire initial
 * Discard buffered partial input.
 */
    void reset() noexcept;
private:
    /**
     * @brief Taille maximale acceptee du contenu d une trame.
     */
    std::size_t max_payload_{};
    /**
     * @brief Octets reçus mais pas encore assembles en trames completes.
     */
    Bytes buffer_{};
};
} // namespace ygg::protocol
