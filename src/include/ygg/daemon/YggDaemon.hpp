#pragma once

/**
 * @file YggDaemon.hpp
 * @brief Point de composition et gestion RAII des services.
 */

#include "ygg/core/Config.hpp"
#include "ygg/core/Result.hpp"
#include <memory>

namespace ygg::daemon {
    
/**
 * @brief Point de composition executant l ensemble des modules.
 * @details Possede les dependances; tous les objets references doivent rester vivants jusqu a fin
 * de l arret.
 * @par Responsabilite du fichier Racine de composition et cycle de vie du processus yggd.
 * @par Regles importantes RAII, arret idempotent, fermeture des sockets et des jobs; eviter les
 * pointeurs pendants durant un shutdown partiel.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Composition root: the ONLY layer wiring network and file services. Starts the secured P2P
 * network (Roots), then optional storage (Seeds) and repair (Regrowth). Owns component lifetimes
 * using RAII.
 */
class YggDaemon final {
public:
    /**
     * @brief Stocke la configuration qui sera validee au demarrage.
     * @param config Configuration a valider avant l usage.
     */
    explicit YggDaemon(core::AppConfig config);
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    ~YggDaemon();
    /**
     * @brief Stocke la configuration qui sera validee au demarrage.
     */
    YggDaemon(const YggDaemon&) = delete;
    /**
     * @brief Transfere la propriete de l etat interne vers cet objet sans dupliquer les secrets ou
     * ressources.
     */
    YggDaemon& operator=(const YggDaemon&) = delete;
    /**
     * @brief Monte les dependances dans l ordre, demarre les services et annule proprement sur
     * echec.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> start();
    /**
     * @brief Arrete l admission, laisse terminer ou annule les travaux puis detruit les dependances
     * inversement.
     */
    void stop() noexcept;
private:
    /**
     * @brief Declaration anticipee de Runtime; la definition/etat se trouve dans l implementation.
     */
    struct Runtime;
    /**
     * @brief Graphe de services du demon possede par une unique_ptr opaque.
     */
    std::unique_ptr<Runtime> runtime_;
    /**
     * @brief Configuration copiee pour le cycle de vie de yggd.
     */
    core::AppConfig config_{};
};

} // namespace ygg::daemon
