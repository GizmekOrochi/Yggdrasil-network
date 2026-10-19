#pragma once

/**
 * @file Domain.hpp
 * @brief Entites immuables, reçus durables et placements distincts.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Types.hpp"
#include "ygg/crypto/CryptoTypes.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace ygg::storage {

/**
 * @brief Description immuable d un fragment chiffre.
 * @details L indice 0 contient A, 1 contient B et 2 contient la parite P=A XOR B; hash et length
 * permettent de detecter les fragments corrompus.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * One encoded XOR share; index 0=A, 1=B, 2=A XOR B.
 */
struct ShardDescriptor {
    /**
     * @brief Identifiant unique du shard, derive selon un format canonique.
     */
    ShardId shard_id{};
    /**
     * @brief Position du segment ou indice du shard dans son groupe de trois.
     */
    std::uint8_t index{};
    /**
     * @brief Empreinte attendue des bytes du shard chiffre complet.
     */
    Digest sha256{};
    /**
     * @brief Longueur declaree des donnees, a verifier a la lecture.
     */
    std::uint32_t length{};
};

/**
 * @brief Description d un segment chiffre et de ses 3 fragments.
 * @details cipher_length permet de reconstruire exactement les octets chiffres apres suppression du
 * padding XOR; chaque nonce de segment doit etre unique par DEK.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Immutable description of an encrypted segment's three shards.
 */
struct SegmentDescriptor {
    /**
     * @brief Position du segment ou indice du shard dans son groupe de trois.
     */
    std::uint64_t index{};
    /**
     * @brief Nonce XChaCha20-Poly1305 du segment, unique pour une meme DEK.
     */
    Nonce24 nonce{};
    /**
     * @brief Longueur exacte du texte chiffre avant padding de la parite.
     */
    std::uint32_t cipher_length{};
    /**
     * @brief Champ shards de SegmentDescriptor : valeur interne conforme aux invariants de ce type.
     */
    std::array<ShardDescriptor,3> shards{};
};

/**
 * @brief Source immuable de verite du contenu et de ses digests.
 * @details Ne comprend pas les positions des pairs; verifie la signature et l encodage canonique
 * avant utilisation.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Signed immutable manifest, encoded canonically before signing. Provider locations are NOT
 * included: they can change without re-signing.
 */
struct SignedManifest {
    /**
     * @brief Identifiant opaque et stable de cette entite.
     */
    ManifestId id{};
    /**
     * @brief Reference du fichier/version concernee.
     */
    FileRef file{};
    /**
     * @brief Cle de signature du proprietaire atteste dans le manifeste.
     */
    PublicKey32 owner_signing_key{};
    /**
     * @brief Champ segments de SignedManifest : valeur interne conforme aux invariants de ce type.
     */
    std::vector<SegmentDescriptor> segments{};
    /**
     * @brief Blob de metadonnees privees deja chiffre et authentifie.
     */
    Bytes encrypted_private_metadata{};
    /**
     * @brief Signature detachee des octets canoniques associes a l objet.
     */
    Signature signature{};
};

/**
 * @brief Association d un descripteur et de ses bytes chiffres.
 * @details Le fournisseur peut stocker ce type sans jamais disposer de la DEK ni du nom de fichier
 * original.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Ciphertext share with manifest-verified description.
 */
struct Shard { 
    /**
     * @brief Donnees du shard : descriptor definit son ID/index/hash; ciphertext_share contient
     * uniquement les bytes chiffres.
     */
    ShardDescriptor descriptor{}; Bytes ciphertext_share{}; 
};

/**
 * @brief Preuve signee qu un shard est durablement stocke.
 * @details Elle atteste shard, host et digest seulement apres verification et synchronisation
 * disque; la signature est verifiee via identite du fournisseur.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Durable shard confirmation signed by its storing node.
 */
struct DurableReceipt {
    /**
     * @brief Identifiant du shard stocke ou declare.
     */
    ShardId shard{};
    /**
     * @brief Identite du noeud revendiquant l hebergement du fragment.
     */
    PeerId host{};
    /**
     * @brief Empreinte attendue des bytes du shard chiffre complet.
     */
    Digest sha256{};
    /**
     * @brief Signature detachee des octets canoniques associes a l objet.
     */
    Signature signature{};
};

/**
 * @brief Preuve de durabilite d une copie de manifeste signe.
 * @details La signature du fournisseur atteste qu il a persiste le manifeste identifie par id.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Durable manifest confirmation signed by the hosting node.
 */
struct ManifestReceipt { 
    /**
     * @brief Identifiant du manifeste, nœud de depot et signature de son accusé durable.
     */
    ManifestId id{}; PeerId host{}; Signature signature{}; 
};

/**
 * @brief Annonce signee d une localisation mutable de shard.
 * @details Une migration ne modifie pas le manifeste : elle ajoute/met a jour les claims verifies.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Mutable location advertisement signed by the provider.
 */
struct PlacementClaim { 
    /**
     * @brief Shard, fournisseur et preuve signee de localisation mutable.
     */
    ShardId shard{}; PeerId host{}; Signature signature{}; 
};

/**
 * @brief Metadonnees client confidentielles pour une version de fichier.
 * @details Ne persiste jamais la DEK nue ni un nom de fichier clair.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Encrypted private name, wrapped key and immutable manifest link.
 */
struct PrivateCatalogEntry {
    /**
     * @brief Reference du fichier/version concernee.
     */
    FileRef file{};
    /**
     * @brief Manifeste signe correspondant au segment examine.
     */
    ManifestId manifest{};
    /**
     * @brief Nom prive chiffre et authentifie cote client.
     */
    Bytes encrypted_name{};
    /**
     * @brief DEK enveloppee pour un destinataire autorise; ne contient pas la DEK nue.
     */
    crypto::WrappedKey wrapped_dek{};
};

/**
 * @brief Triple de nœuds distincts pour un segment XOR 2+1.
 * @details Chacun doit appartenir a un domaine de panne physique different, pas seulement avoir une
 * IP differente.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Three independent physical placement destinations.
 */
struct Nodes3 { std::array<PeerId,3> peers{}; };

/**
 * @brief Capacite et domaine de panne annonces par un candidat.
 * @details free_bytes est indicatif et ne remplace pas une reservation ou un retour de quota cote
 * store.
 * @par Responsabilite du fichier Modele immuable du stockage distribue.
 * @par Regles importantes Un manifeste ne contient pas d adresses de stockage mutables; hash et
 * signature reposent sur une representation canonique explicitement versionnee.

 * @par Contrat complementaire initial
 * Reported candidate capacity is advisory until verified.
 */
struct NodeCapacity {
    /**
     * @brief Identifiant du pair concerne, jamais substitut aux preuves d identite.
     */
    PeerId peer{};
    /**
     * @brief Espace libre annonce, indicatif tant qu aucun quota n est reserve.
     */
    std::uint64_t free_bytes{};
    /**
     * @brief Groupe physique de panne commun (machine/disque), utilise pour eviter la
     * colocalisation.
     */
    std::string failure_domain{};
};

} // namespace ygg::storage
