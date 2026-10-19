#pragma once

/**
 * @file Xor2Plus1Codec.hpp
 * @brief Algorithme XOR 2+1 sur donnees chiffrées.
 */

#include "ygg/storage/IErasureCodec.hpp"

namespace ygg::storage {

/**
 * @brief Composant Xor2Plus1Codec du module storage.
 * @details Segmente le texte chiffre en A et B, produit P=A XOR B, puis reconstruit avec deux
 * shards d indices differents.
 * @par Responsabilite du fichier Implementation XOR 2+1 du codec.
 * @par Regles importantes Verifier taille, indices disponibles et padding; sans verification
 * cryptographique du hash, le XOR n atteste aucune authenticite.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Minimal XOR erasure codec: A + B, parity P = A XOR B.
 */
class Xor2Plus1Codec final : public IErasureCodec {
public:
    /**
     * @brief Construit A, B et P=A XOR B avec padding de la derniere demi-partie si besoin.
     * @param ciphertext Octets chiffres incluant le tag AEAD si applicable.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<std::array<Bytes,3>> encode(ByteView ciphertext) const override;
    /**
     * @brief Selectionne deux shards distincts, reconstruit celui absent et retire le padding grace
     * a cipher_length.
     * @param shards Fragments a decoder ou client RPC injecte selon la signature.
     * @param cipher_length Longueur exacte du ciphertext avant padding XOR.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<Bytes> reconstruct(const std::array<std::optional<Bytes>,3>& shards, std::size_t cipher_length) const override;
};
} // namespace ygg::storage
