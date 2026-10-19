#pragma once

/**
 * @file Clock.hpp
 * @brief Horloge monotone injectable pour tests.
 */

#include "ygg/core/Types.hpp"

namespace ygg::core {

/**
 * @brief Composant IClock du module core.
 * @details Fournit une reference temporelle stable aux expirations, delais de grace et tentatives
 * de reconnexion.
 * @par Responsabilite du fichier Horloge monotone injectable.
 * @par Regles importantes Ne pas employer une heure murale modifiable pour des deadlines. Les tests
 * injectent une horloge simulee via IClock.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Testable monotonic clock for deadlines and grace periods.
 */
class IClock {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IClock() = default;
    /**
     * @brief Retourne l instant monotone courant, comparable a un MonotonicTime issu de la meme
     * horloge.
     * @return Instant monotone, independant des corrections de l heure systeme.
     */
    [[nodiscard]] virtual MonotonicTime now() const noexcept = 0;
};

/**
 * @brief Composant SteadyClock du module core.
 * @details Fournit une reference temporelle stable aux expirations, delais de grace et tentatives
 * de reconnexion.
 * @par Responsabilite du fichier Horloge monotone injectable.
 * @par Regles importantes Ne pas employer une heure murale modifiable pour des deadlines. Les tests
 * injectent une horloge simulee via IClock.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Clock adapter backed by std::chrono::steady_clock.
 */
class SteadyClock final : public IClock {
public:
    /**
     * @brief Lit std::chrono::steady_clock::now() sans effet secondaire et sans allocation.
     * @return Instant monotone, independant des corrections de l heure systeme.
     */
    [[nodiscard]] MonotonicTime now() const noexcept override;
};
} // namespace ygg::core
