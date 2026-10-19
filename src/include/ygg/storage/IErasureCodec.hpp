#pragma once

/**
 * @file IErasureCodec.hpp
 * @brief Port de codage correcteur des fragments chiffrés.
 */

#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <array>
#include <optional>
#include <cstddef>

namespace ygg::storage {
    
/**
 * @brief Composant IErasureCodec du module storage.
 * @details Transforme un segment chiffre en trois shards XOR 2+1 et reconstruit le texte chiffre
 * avec deux indices distincts.
 * @par Responsabilite du fichier Port de codage d effacement independant du reseau.
 * @par Regles importantes La taille originale du chiffre determine le retrait du padding. Le codec
 * ne chiffre ni ne verifie de signature.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Encode ciphertext only; workers never use file-decryption keys. For XOR 2+1, any two DISTINCT
 * valid shard indices must reconstruct the ciphertext up to the exact authenticated original
 * cipher length.
 */
class IErasureCodec {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IErasureCodec() = default;
    /**
     * @brief Divise le ciphertext en deux moities A/B et ajoute le XOR P de meme longueur.
     * @param ciphertext Octets chiffres incluant le tag AEAD si applicable.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<std::array<Bytes,3>> encode(ByteView ciphertext) const = 0;
    /**
     * @brief Avec au moins deux indices distincts presents, recalcule le ciphertext de longueur
     * exacte.
     * @param shards Fragments a decoder ou client RPC injecte selon la signature.
     * @param cipher_length Longueur exacte du ciphertext avant padding XOR.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<Bytes> reconstruct(const std::array<std::optional<Bytes>,3>& shards, std::size_t cipher_length) const = 0;
};
} // namespace ygg::storage
