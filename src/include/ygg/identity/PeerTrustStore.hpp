#pragma once

/**
 * @file PeerTrustStore.hpp
 * @brief Autorisation persistante des pairs par cle.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"

namespace ygg::identity {

/**
 * @brief Liste des etats/codes stables de TrustState.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Registre local des autorisations et revocations.
 * @par Regles importantes Une annonce mDNS et un HELLO ne valent jamais approbation. Toute
 * variation de cle pour un meme PeerId doit etre traitee comme suspecte.

 * @par Contrat complementaire initial
 * Explicit local pinning decision.
 */
enum class TrustState {
    /**
     * @brief Aucune approbation; la connexion doit etre refusee.
     */
    unknown,
    /**
     * @brief Cle exacte autorisee explicitement.
     */
    approved,
    /**
     * @brief Pair interdit meme s il etait approuve avant.
     */
    blocked
};

/**
 * @brief Structure de donnees TrustEntry utilisee par identity.
 * @details Associe PeerId, cle publique exacte et decision explicite inconnue/approuvee/bloquee.
 * @par Responsabilite du fichier Registre local des autorisations et revocations.
 * @par Regles importantes Une annonce mDNS et un HELLO ne valent jamais approbation. Toute
 * variation de cle pour un meme PeerId doit etre traitee comme suspecte.
 */
struct TrustEntry {
    /**
     * @brief Identifiant opaque et stable de cette entite.
     */
    PeerId id{};
    /**
     * @brief Cle publique du proprietaire/de pair; ne contient aucun secret.
     */
    PublicKey32 public_key{};
    /**
     * @brief Statut de sante ou de travail a persister selon la structure.
     */
    TrustState state{TrustState::unknown};
};

/**
 * @brief Source locale de verite des autorisations.
 * @details Le truststore ne doit jamais considerer un PeerId seul comme preuve d identite : la cle
 * doit aussi correspondre.
 * @par Responsabilite du fichier Registre local des autorisations et revocations.
 * @par Regles importantes Une annonce mDNS et un HELLO ne valent jamais approbation. Toute
 * variation de cle pour un meme PeerId doit etre traitee comme suspecte.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Durable peer authorization; mDNS results never implicitly approve. Out-of-band verification of
 * fingerprints is required for approval.
 */
class PeerTrustStore final {
public:
    /**
     * @brief Configure la base locale contenant les decisions d autorisation persistantes.
     * @param database_path Emplacement de la base de confiance locale.
     */
    explicit PeerTrustStore(Path database_path);
    /**
     * @brief Associe durablement PeerId et la cle exacte apres confirmation externe de leur
     * empreinte.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @param key Cle publique correspondant exactement au PairId.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> approve(PeerId id, PublicKey32 key);
    /**
     * @brief Marque un PairId comme interdit et rend toutes ses futures connexions non autorisees.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> block(PeerId id);
    /**
     * @brief Charge la decision d autorisation et la cle associees a un PairId, ou signale l
     * absence.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<TrustEntry> lookup(PeerId id) const;
    /**
     * @brief N accorde vrai que si PeerId ET cle publique correspondent a une entree approuvee.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @param key Cle publique correspondant exactement au PairId.
     * @return Vrai si la condition est remplie, faux sinon; aucun acces aux secrets.
     */
    [[nodiscard]] bool is_approved(PeerId id, PublicKey32 key) const;
private:
    /**
     * @brief Chemin persistant vers la base de confiance.
     */
    Path database_path_;
};

} // namespace ygg::identity
