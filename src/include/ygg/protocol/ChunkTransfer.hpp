#pragma once

/**
 * @file ChunkTransfer.hpp
 * @brief Transfert de chunks avec limites et backpressure.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <cstdint>

namespace ygg::protocol {

/**
 * @brief Structure de donnees TransferChunk utilisee par protocol.
 * @details Valide les offsets, gere l accusé de reception, applique une fenetre de flux et fournit
 * un digest final.
 * @par Responsabilite du fichier Transfert de gros objets par morceaux bornes.
 * @par Regles importantes Une validation par chunk ne remplace pas la verification du hash du shard
 * entier; limiter les ressources par transfert.

 * @par Contrat complementaire initial
 * Position of one transfer chunk in an existing RPC stream.
 */
struct TransferChunk { 
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    std::uint64_t offset{}; Bytes payload{}; 
};

/**
 * @brief Structure de donnees ChunkAck utilisee par protocol.
 * @details Valide les offsets, gere l accusé de reception, applique une fenetre de flux et fournit
 * un digest final.
 * @par Responsabilite du fichier Transfert de gros objets par morceaux bornes.
 * @par Regles importantes Une validation par chunk ne remplace pas la verification du hash du shard
 * entier; limiter les ressources par transfert.

 * @par Contrat complementaire initial
 * Acknowledgement for a verified chunk.
 */
struct ChunkAck { 
    /**
     * @brief Offset que le receveur attend pour le morceau suivant.
     */
    std::uint64_t next_offset{};
};

/**
 * @brief Composant ChunkTransfer du module protocol.
 * @details Valide les offsets, gere l accusé de reception, applique une fenetre de flux et fournit
 * un digest final.
 * @par Responsabilite du fichier Transfert de gros objets par morceaux bornes.
 * @par Regles importantes Une validation par chunk ne remplace pas la verification du hash du shard
 * entier; limiter les ressources par transfert.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Transport-neutral bounded chunk receiver with backpressure.
 */
class ChunkTransfer final {
public:
    /**
     * @brief Configure taille attendue et limite de fenetre du transfert.
     * @param expected_length Nombre total d octets que le transfert doit contenir.
     * @param window_size Capacite maximale de la fenetre de transfert.
     */
    ChunkTransfer(std::uint64_t expected_length, std::uint32_t window_size);
    /**
     * @brief Valide ordre/offset et taille d un morceau, applique le controle de flux puis accuse
     * reception.
     * @param chunk Morceau situe a un offset precis du transfert.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @warning Refuser les chevauchements, les trous non geres et les offsets au-dela de
     * expected_length_.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<ChunkAck> receive(TransferChunk chunk);
    /**
     * @brief Confirme la longueur totale attendue et fournit un digest du flux assemble.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<Digest> finalize();
    /**
     * @brief Arrete le transfert et supprime les buffers temporaires en cours.
     */
    void cancel() noexcept;
private:
    /**
     * @brief Longueur totale attendue du transfert en chunks.
     */
    std::uint64_t expected_length_{};
    /**
     * @brief Plafond de la fenetre d envoi/reception non acquittee.
     */
    std::uint32_t window_size_{};
};
} // namespace ygg::protocol
