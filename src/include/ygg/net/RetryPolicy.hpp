#pragma once

/**
 * @file RetryPolicy.hpp
 * @brief Strategie de reconnexion limitee.
 */

#include "ygg/core/Types.hpp"
#include <cstdint>

namespace ygg::net {
    
/**
 * @brief Composant RetryPolicy du module net.
 * @details Calcule des delais de backoff croissants, bornes, avec jitter pour prevenir la
 * synchronisation des pairs.
 * @par Responsabilite du fichier Politique de reessai de connexion.
 * @par Regles importantes Ne jamais reessayer automatiquement apres une decision de refus explicite
 * ou une erreur de confiance.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Bounded exponential backoff with jitter after network errors.
 */
class RetryPolicy final {
public:
    /**
     * @brief Definit une duree initiale, un maximum et le nombre maximal de tentatives.
     * @param initial Premier delai de backoff.
     * @param maximum Delai maximum autorise pour un reessai.
     * @param attempts Nombre maximal de tentatives admises.
     */
    RetryPolicy(Duration initial, Duration maximum, std::uint32_t attempts);
    /**
     * @brief Calcule un delai exponentiel borne et avec jitter pour la tentative numero attempt.
     * @param attempt Numero de tentative de reconnexion.
     * @return Delai en millisecondes a employer pour la prochaine tentative.
     */
    [[nodiscard]] Duration next_delay(std::uint32_t attempt) const;
    /**
     * @brief Refuse les tentatives au-dela de la limite configuree.
     * @param attempt Numero de tentative de reconnexion.
     * @return Vrai si la condition est remplie, faux sinon; aucun acces aux secrets.
     */
    [[nodiscard]] bool may_retry(std::uint32_t attempt) const noexcept;
private:
    /**
     * @brief Duree initiale du backoff.
     */
    Duration initial_{};
    /**
     * @brief Duree maximale de backoff.
     */
    Duration maximum_{};
    /**
     * @brief Nombre maximal de reessais autorises.
     */
    std::uint32_t attempts_{};
};

} // namespace ygg::net
