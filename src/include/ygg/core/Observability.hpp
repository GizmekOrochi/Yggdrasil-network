#pragma once

/**
 * @file Observability.hpp
 * @brief Journalisation structuree sans divulgation de secrets.
 */

#include "ygg/core/Ids.hpp"
#include <functional>
#include <optional>
#include <string>

namespace ygg::core {

/**
 * @brief Liste des etats/codes stables de LogLevel.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Observabilite sans secrets.
 * @par Regles importantes Ne jamais journaliser des DEK, secrets, contenus en clair, noms prives ou
 * donnees sensibles dans les messages et les categories.

 * @par Contrat complementaire initial
 * Severity of structured, secret-free diagnostic events.
 */
enum class LogLevel {
    /**
     * @brief Details developpeur tres fins sans secrets.
     */
    trace,
    /**
     * @brief Etat utile au debogage.
     */
    debug,
    /**
     * @brief Evenement normal d exploitation.
     */
    info,
    /**
     * @brief Situation recuperable exigeant une surveillance.
     */
    warning,
    /**
     * @brief Echec d operation.
     */
    error
};

/**
 * @brief Structure de donnees Event utilisee par core.
 * @details Abstrait les evenements diagnostiques pour la CLI, la journalisation ou des outils de
 * supervision.
 * @par Responsabilite du fichier Observabilite sans secrets.
 * @par Regles importantes Ne jamais journaliser des DEK, secrets, contenus en clair, noms prives ou
 * donnees sensibles dans les messages et les categories.

 * @par Contrat complementaire initial
 * One structured event; never include DEKs or clear file names.
 */
struct Event {
    /**
     * @brief Severite de l evenement observabilite.
     */
    LogLevel level{LogLevel::info};
    /**
     * @brief Domaine fonctionnel de l evenement, sans donnees sensibles.
     */
    std::string category{};
    /**
     * @brief Information de diagnostic ne devant jamais contenir de secret.
     */
    std::string message{};
    /**
     * @brief Identifiant du pair concerne, jamais substitut aux preuves d identite.
     */
    std::optional<PeerId> peer{};
};

/**
 * @brief Composant IObservability du module core.
 * @details Abstrait les evenements diagnostiques pour la CLI, la journalisation ou des outils de
 * supervision.
 * @par Responsabilite du fichier Observabilite sans secrets.
 * @par Regles importantes Ne jamais journaliser des DEK, secrets, contenus en clair, noms prives ou
 * donnees sensibles dans les messages et les categories.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Pluggable diagnostics interface.
 */
class IObservability {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IObservability() = default;
    /**
     * @brief Informe la sortie diagnostique d un evenement deja nettoye de ses secrets.
     * @param entry Evenement ou entree a persister selon le contexte.
     */
    virtual void event(const Event& entry) = 0;
};

/**
 * @brief Composant Observability du module core.
 * @details Abstrait les evenements diagnostiques pour la CLI, la journalisation ou des outils de
 * supervision.
 * @par Responsabilite du fichier Observabilite sans secrets.
 * @par Regles importantes Ne jamais journaliser des DEK, secrets, contenus en clair, noms prives ou
 * donnees sensibles dans les messages et les categories.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Default adapter to an application-selected log sink.
 */
class Observability final : public IObservability {
public:
    /**
     * @brief Enregistre le sink injectable qui recevra les evenements diagnostiques sans logique
     * reseau.
     */
    explicit Observability(std::function<void(const Event&)> sink);
    /**
     * @brief Transmet l evenement structure au sink apres application des regles de
     * confidentialite.
     * @param entry Evenement ou entree a persister selon le contexte.
     */
    void event(const Event& entry) override;
private:
    /**
     * @brief Operation du contrat Observability ; les garanties de son interface et de son module
     * doivent etre respectees.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    std::function<void(const Event&)> sink_;
};

} // namespace ygg::core
