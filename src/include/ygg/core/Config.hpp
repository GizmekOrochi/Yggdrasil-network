#pragma once

/**
 * @file Config.hpp
 * @brief Configuration du daemon et validation.
 */

#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include "ygg/core/Ids.hpp"
#include <cstdint>
#include <string>

namespace ygg::core {
    
/**
 * @brief Configuration complete de yggd en un objet valide.
 * @details L instance doit etre validee avant l ouverture des sockets et l acces aux donnees
 * durables.
 * @par Responsabilite du fichier Parametres d exploitation de chaque demon.
 * @par Regles importantes Verifier permissions, plages, chemins et collisions avant de demarrer.
 * Aucune option ne doit neutraliser implicitement TLS ou l autorisation des pairs.

 * @par Contrat complementaire initial
 * Paths and runtime parameters of a local yggd instance.
 */
struct AppConfig {
    /**
     * @brief Racine locale des donnees persistantes de yggd.
     */
    Path data_root{};
    /**
     * @brief Chemin durable des cles de l identite locale.
     */
    Path identity_path{};
    /**
     * @brief Chemin des decisions d approbation persistantes.
     */
    Path trust_store_path{};
    /**
     * @brief Socket Unix reserve aux clients locaux autorises.
     */
    Path local_socket_path{};
    /**
     * @brief Hote/port TCP sur lesquels le nœud attend les pairs.
     */
    Endpoint listen_endpoint{};
    /**
     * @brief Libelle visible sans valeur d authentification.
     */
    std::string display_name{};
    /**
     * @brief Quota maximal de shards que ce nœud accepte de stocker.
     */
    std::uint64_t shard_quota_bytes{};
    /**
     * @brief Deadline relative autorisee pour etablir une connexion.
     */
    Duration connect_timeout{5000};
    /**
     * @brief Duree avant lancement d une reparation apres perte de disponibilite.
     */
    Duration repair_grace_period{60000};
    /**
     * @brief Active ou desactive la recherche automatique de candidats LAN.
     */
    bool mdns_enabled{true};
};

/**
 * @brief Composant ConfigLoader du module core.
 * @details Decrit les chemins de travail, le point d ecoute, les quotas, les delais et les
 * preferences de decouverte.
 * @par Responsabilite du fichier Parametres d exploitation de chaque demon.
 * @par Regles importantes Verifier permissions, plages, chemins et collisions avant de demarrer.
 * Aucune option ne doit neutraliser implicitement TLS ou l autorisation des pairs.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Configuration parser and validator; never silently disables TLS.
 */
class ConfigLoader {
public:

    /**
     * @brief Ouvre et parse un fichier TOML, instancie AppConfig puis refuse la configuration
     * malformee.
     * @param path Chemin local de la ressource concernee.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Read a TOML configuration file. @param path File to load.
 */
    [[nodiscard]] Result<AppConfig> load(const Path& path) const;

    /**
     * @brief Valide coherences, limites et securite des chemins; normalise si necessaire sans
     * accepter silencieusement un mode non protege.
     * @param config Configuration a valider avant l usage.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @warning Aucun changement de politique cryptographique implicite n est permis.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Check runtime limits and canonicalize necessary paths.
 */
    [[nodiscard]] Result<void> validate(const AppConfig& config) const;
};
} // namespace ygg::core
