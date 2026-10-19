#pragma once

/**
 * @file Types.hpp
 * @brief Types binaires, contraintes de taille et temps monotone.
 */

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <span>
#include <vector>

namespace ygg {

/**
 * @brief Buffer binaire possede et deplacable, sure pour les coroutines.

 * @par Contrat complementaire initial
 * Owning binary buffer safe across coroutine suspension.
 */
using Bytes = std::vector<std::byte>;

/**
 * @brief Vue non possedee; ne pas memoriser au-dela de l appel synchrone.

 * @par Contrat complementaire initial
 * Borrowed buffer only valid during synchronous operations.
 */
using ByteView = std::span<const std::byte>;
/**
 * @brief Duree en millisecondes, pour backoff, connexion ou delai de grace.
 */
using Duration = std::chrono::milliseconds;
/**
 * @brief Instant stable compare exclusivement via horloge monotone.
 */
using MonotonicTime = std::chrono::steady_clock::time_point;
/**
 * @brief Chemin du systeme de fichiers local.
 */
using Path = std::filesystem::path;

/**
 * @brief Structure de donnees Limits utilisee par core.
 * @details Distingue buffers possedes, vues empruntees, chemins locaux et instants monotones.
 * @par Responsabilite du fichier Types de donnees, durées et limites transversales.
 * @par Regles importantes ByteView ne doit jamais survivre a une suspension co_await. Les limites
 * sont des plafonds protocolaires, pas une protection contre une exhaustion memoire cumulee.

 * @par Contrat complementaire initial
 * Limits chosen by the UML specification.
 */
struct Limits {
    /**
     * @brief Champ max_control_bytes de Limits : valeur interne conforme aux invariants de ce type.
     */
    static constexpr std::size_t max_control_bytes = 1024u * 1024u;
    /**
     * @brief Champ max_chunk_bytes de Limits : valeur interne conforme aux invariants de ce type.
     */
    static constexpr std::size_t max_chunk_bytes = 256u * 1024u;
    /**
     * @brief Champ segment_plain_bytes de Limits : valeur interne conforme aux invariants de ce
     * type.
     */
    static constexpr std::size_t segment_plain_bytes = 2u * 1024u * 1024u;
    /**
     * @brief Champ xor_shards de Limits : valeur interne conforme aux invariants de ce type.
     */
    static constexpr std::size_t xor_shards = 3u;
    /**
     * @brief Champ xor_required de Limits : valeur interne conforme aux invariants de ce type.
     */
    static constexpr std::size_t xor_required = 2u;
};

} // namespace ygg
