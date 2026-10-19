#pragma once

/**
 * @file Ids.hpp
 * @brief Identifiants forts et valeurs reseau fondamentales.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace ygg {
/** @brief Opaque 256-bit identifier with compile-time category separation. */

template <typename Tag>
/**
 * @brief Identifiant opaque de 256 bits distingue par son Tag.
 * @details Les valeurs zero ne representent pas par convention un identifiant valide; les
 * generateurs doivent assurer unicite et separation de domaine.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.
 */
struct StrongId {
    /**
     * @brief Octets immuables representant un identifiant, digest ou buffer selon son type.
     */
    std::array<std::byte, 32> bytes{};
    friend bool operator==(const StrongId&, const StrongId&) = default;
};

/**
 * @brief Declaration anticipee de PeerTag; la definition/etat se trouve dans l implementation.
 */
struct PeerTag;
/**
 * @brief Declaration anticipee de FileTag; la definition/etat se trouve dans l implementation.
 */
struct FileTag;
/**
 * @brief Declaration anticipee de VersionTag; la definition/etat se trouve dans l implementation.
 */
struct VersionTag;
/**
 * @brief Declaration anticipee de ManifestTag; la definition/etat se trouve dans l implementation.
 */
struct ManifestTag;
/**
 * @brief Declaration anticipee de ShardTag; la definition/etat se trouve dans l implementation.
 */
struct ShardTag;
/**
 * @brief Declaration anticipee de RequestTag; la definition/etat se trouve dans l implementation.
 */
struct RequestTag;
/**
 * @brief Declaration anticipee de SnapshotTag; la definition/etat se trouve dans l implementation.
 */
struct SnapshotTag;

/**
 * @brief Identifiant fort PeerId, distinct des autres identifiants a la compilation.
 */
using PeerId = StrongId<PeerTag>;
/**
 * @brief Identifiant fort FileId, distinct des autres identifiants a la compilation.
 */
using FileId = StrongId<FileTag>;
/**
 * @brief Identifiant fort VersionId, distinct des autres identifiants a la compilation.
 */
using VersionId = StrongId<VersionTag>;
/**
 * @brief Identifiant fort ManifestId, distinct des autres identifiants a la compilation.
 */
using ManifestId = StrongId<ManifestTag>;
/**
 * @brief Identifiant fort ShardId, distinct des autres identifiants a la compilation.
 */
using ShardId = StrongId<ShardTag>;
/**
 * @brief Identifiant fort RequestId, distinct des autres identifiants a la compilation.
 */
using RequestId = StrongId<RequestTag>;
/**
 * @brief Identifiant fort SnapshotId, distinct des autres identifiants a la compilation.
 */
using SnapshotId = StrongId<SnapshotTag>;

/**
 * @brief Structure de donnees Digest utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.

 * @par Contrat complementaire initial
 * SHA-256 digest of encoded bytes.
 */
struct Digest {
    /**
     * @brief Octets immuables representant un identifiant, digest ou buffer selon son type.
     */
    std::array<std::byte, 32> bytes{};
};

/**
 * @brief Structure de donnees Nonce24 utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.

 * @par Contrat complementaire initial
 * XChaCha20 nonce. Must never be reused for the same key.
 */
struct Nonce24 {
    /**
     * @brief Octets immuables representant un identifiant, digest ou buffer selon son type.
     */
    std::array<std::byte, 24> bytes{};
};

/**
 * @brief Structure de donnees Signature utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.

 * @par Contrat complementaire initial
 * Ed25519 detached signature.
 */
struct Signature {
    /**
     * @brief Octets immuables representant un identifiant, digest ou buffer selon son type.
     */
    std::array<std::byte, 64> bytes{};
};

/**
 * @brief Structure de donnees PublicKey32 utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.
 */
struct PublicKey32 {
    /**
     * @brief Octets immuables representant un identifiant, digest ou buffer selon son type.
     */
    std::array<std::byte, 32> bytes{};
};

/**
 * @brief Structure de donnees ServiceId utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.
 */
struct ServiceId {
    /**
     * @brief Champ value de ServiceId : valeur interne conforme aux invariants de ce type.
     */
    std::string value{};
};

/**
 * @brief Structure de donnees Endpoint utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.
 */
struct Endpoint {
    /**
     * @brief Identite du noeud revendiquant l hebergement du fragment.
     */
    std::string host{};
    /**
     * @brief Port TCP distant ou local; valide dans l intervalle 1..65535 pour une connexion.
     */
    std::uint16_t port{};
};

/**
 * @brief Structure de donnees FileRef utilisee par core.
 * @details Evite de confondre identite de pair, fichier, version, manifeste, fragment, requete et
 * sauvegarde.
 * @par Responsabilite du fichier Identifiants cryptographiques et valeurs fortes.
 * @par Regles importantes Les 32 octets d un StrongId ne suffisent pas a definir sa derivation.
 * Fixer un encodage canonique et des domaines cryptographiques avant echange sur le fil.
 */
struct FileRef {
    /**
     * @brief Identifiant opaque et stable de cette entite.
     */
    FileId id{};
    /**
     * @brief Version precise d un fichier; ne pas confondre avec le numero de protocole.
     */
    VersionId version{};
};

} // namespace ygg
