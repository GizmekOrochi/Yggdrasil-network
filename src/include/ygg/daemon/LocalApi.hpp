#pragma once

/**
 * @file LocalApi.hpp
 * @brief Messages de controle local yggctl vers yggd.
 */

#include "ygg/core/Result.hpp"
#include "ygg/core/Ids.hpp"
#include "ygg/core/Types.hpp"
#include <optional>
#include <string>

namespace ygg::daemon {

/**
 * @brief Liste des etats/codes stables de LocalCommandType.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Modele des commandes et reponses du socket Unix local.
 * @par Regles importantes Validation par type de commande obligatoire : les champs optionnels ne
 * sont pas interchangeables; la presence de champs inattendus est a definir.

 * @par Contrat complementaire initial
 * Unprivileged CLI command over a permission-restricted Unix socket.
 */
enum class LocalCommandType {
    /**
     * @brief Demande l inventaire local des nœuds actifs.
     */
    list_peers,
    /**
     * @brief Demande un dial manuel sur endpoint.
     */
    connect,
    /**
     * @brief Enregistre une autorisation apres verification hors bande.
     */
    trust_approve,
    /**
     * @brief Revoque une autorisation locale.
     */
    trust_block,
    /**
     * @brief Envoie un message de service au pair cible.
     */
    send_message,
    /**
     * @brief Importe un fichier local chiffre.
     */
    put_file,
    /**
     * @brief Restaure une version de fichier vers un chemin local.
     */
    get_file
};

/**
 * @brief Demande de yggctl a yggd par socket Unix.
 * @details Chaque LocalCommandType definit les champs requis et exclus; les champs optionnels ne
 * doivent pas etre devines.
 * @par Responsabilite du fichier Modele des commandes et reponses du socket Unix local.
 * @par Regles importantes Validation par type de commande obligatoire : les champs optionnels ne
 * sont pas interchangeables; la presence de champs inattendus est a definir.
 */
struct LocalCommand {
    /**
     * @brief Champ type de LocalCommand : valeur interne conforme aux invariants de ce type.
     */
    LocalCommandType type{LocalCommandType::list_peers};
    /**
     * @brief Identifiant du pair concerne, jamais substitut aux preuves d identite.
     */
    std::optional<PeerId> peer{};
    /**
     * @brief Reference du fichier/version concernee.
     */
    std::optional<FileRef> file{};
    /**
     * @brief Adresse candidate qui doit etre validee lors du dial.
     */
    std::optional<Endpoint> endpoint{};
    /**
     * @brief Champ argument de LocalCommand : valeur interne conforme aux invariants de ce type.
     */
    std::string argument{};
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    Bytes payload{};
};

/**
 * @brief Reponse du demon a une commande locale.
 * @details success et error doivent rester cohérents; payload ne peut contenir de secrets non
 * autorises.
 * @par Responsabilite du fichier Modele des commandes et reponses du socket Unix local.
 * @par Regles importantes Validation par type de commande obligatoire : les champs optionnels ne
 * sont pas interchangeables; la presence de champs inattendus est a definir.
 */
struct LocalResponse {
    /**
     * @brief Champ success de LocalResponse : valeur interne conforme aux invariants de ce type.
     */
    bool success{};
    /**
     * @brief Champ error de LocalResponse : valeur interne conforme aux invariants de ce type.
     */
    std::optional<Error> error{};
    /**
     * @brief Octets applicatifs possedes, non interpretes par les couches reseau.
     */
    Bytes payload{};
};
} // namespace ygg::daemon
