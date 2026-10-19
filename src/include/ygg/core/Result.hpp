#pragma once

/**
 * @file Result.hpp
 * @brief Erreurs typees, Result et type de coroutine C++20.
 */

#include <boost/asio/awaitable.hpp>
#include <optional>
#include <string>
#include <variant>
#include <utility>

namespace ygg {

/**
 * @brief Liste des etats/codes stables de ErrorCode.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Contrat commun d erreurs explicites.
 * @par Regles importantes Aucune exception ne doit traverser les frontieres des modules. Definir
 * les preconditions d acces value()/error() et traduire les echecs systeme en ErrorCode.

 * @par Contrat complementaire initial
 * Stable error category independent of third-party libraries.
 */
enum class ErrorCode {
    /**
     * @brief Entree absente, hors bornes ou mal formee.
     */
    invalid_input,
    /**
     * @brief Operation interdite dans l etat courant.
     */
    invalid_state,
    /**
     * @brief Taille filaire ou buffer depassant les limites.
     */
    oversized_message,
    /**
     * @brief Version ou type de protocole incompatible.
     */
    protocol_mismatch,
    /**
     * @brief Action refusee pour ce client/pair.
     */
    unauthorized,
    /**
     * @brief Identite du pair non approuvee.
     */
    peer_untrusted,
    /**
     * @brief Preuve cryptographique ou TLS invalide.
     */
    authentication_failed,
    /**
     * @brief Canal devenu indisponible ou ferme.
     */
    disconnected,
    timeout, busy,
    /**
     * @brief Ressource temporairement inaccessible.
     */
    unavailable,
    /**
     * @brief Moins de deux indices de shards valides pour XOR 2+1.
     */
    insufficient_shards,
    /**
     * @brief Hash, tag AEAD ou signature invalide.
     */
    integrity_error,
    /**
     * @brief Cle necessaire absente chez le client.
     */
    missing_key,
    /**
     * @brief Pas assez de stockage ou destinations independantes.
     */
    insufficient_capacity,
    /**
     * @brief Quota du nœud depasse.
     */
    quota_exceeded,
    /**
     * @brief Collision identifiant/version/operation concurrente.
     */
    conflict,
    /**
     * @brief Echec de systeme de fichiers ou autre I/O.
     */
    io_error,
    /**
     * @brief Fonction ou suite cryptographique non prise en charge.
     */
    unsupported,
    /**
     * @brief Invariant de programmation ou erreur inattendue.
     */
    internal_error
};

/**
 * @brief Echec explicite transportable entre les couches.
 * @details Le message est destine au diagnostic, jamais a une fuite de donnees sensibles.
 * @par Responsabilite du fichier Contrat commun d erreurs explicites.
 * @par Regles importantes Aucune exception ne doit traverser les frontieres des modules. Definir
 * les preconditions d acces value()/error() et traduire les echecs systeme en ErrorCode.

 * @par Contrat complementaire initial
 * Explicit error returned across module boundaries.
 */
struct Error {
    /**
     * @brief Categorie d erreur stable et independante des exceptions des bibliotheques externes.
     */
    ErrorCode code{ErrorCode::internal_error};
    /**
     * @brief Information de diagnostic ne devant jamais contenir de secret.
     */
    std::string message{};
};

/**
 * @brief Result value or typed error; no exceptions cross module boundaries.
 *
 * Implementations are intentionally absent from the header skeleton.
 */
template <typename T>
/**
 * @brief Conteneur d une valeur valide OU d une erreur typée.
 * @details L implementation doit rendre toute lecture de la mauvaise branche detectable; les
 * fonctions statiques sont les seuls constructeurs publics.
 * @par Responsabilite du fichier Contrat commun d erreurs explicites.
 * @par Regles importantes Aucune exception ne doit traverser les frontieres des modules. Definir
 * les preconditions d acces value()/error() et traduire les echecs systeme en ErrorCode.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.
 */
class Result {
public:
    /**
     * @brief Construit le resultat dans sa variante de succes; la valeur est transferee par
     * mouvement si possible.
     * @param value Valeur transportee dans le resultat de succes.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    static Result success(T value);
    /**
     * @brief Construit le resultat dans sa variante d echec en conservant ErrorCode et diagnostic.
     * @param error Erreur typee decrivant un echec.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    static Result failure(Error error);
    /**
     * @brief Indique si la variante contient une valeur valide, sans modifier le resultat.
     * @return Vrai si la condition est remplie, faux sinon; aucun acces aux secrets.
     */
    [[nodiscard]] bool ok() const noexcept;
    /**
     * @brief Accede a la valeur uniquement lorsque ok() est vrai; definir une politique de
     * precondition explicite.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     * @pre ok() == true ; ne jamais lire cette branche sur un echec.
     */
    [[nodiscard]] const T& value() const;
    /**
     * @brief Accede a la valeur uniquement lorsque ok() est vrai; definir une politique de
     * precondition explicite.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     * @pre ok() == true ; ne jamais lire cette branche sur un echec.
     */
    [[nodiscard]] T& value();
    /**
     * @brief Accede a l erreur uniquement lorsque ok() est faux; definir une politique de
     * precondition explicite.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     * @pre ok() == false ; ne jamais lire cette branche sur un succes.
     */
    [[nodiscard]] const Error& error() const;
private:
    /**
     * @brief Variante possedant exactement une valeur ou une erreur.
     */
    std::variant<T, Error> data_;
};

/** @brief Result specialization for operations without a return value. */
template <>
/**
 * @brief Specialisation du resultat pour une operation sans valeur.
 * @details L absence d erreur dans optional<Error> represente le succes, et sa presence un echec.
 * @par Responsabilite du fichier Contrat commun d erreurs explicites.
 * @par Regles importantes Aucune exception ne doit traverser les frontieres des modules. Definir
 * les preconditions d acces value()/error() et traduire les echecs systeme en ErrorCode.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.
 */
class Result<void> {
public:
    /**
     * @brief Construit le resultat void de succes sans valeur ni erreur.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    static Result success();
    /**
     * @brief Construit le resultat dans sa variante d echec en conservant ErrorCode et diagnostic.
     * @param error Erreur typee decrivant un echec.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     */
    static Result failure(Error error);
    /**
     * @brief Indique si la variante contient une valeur valide, sans modifier le resultat.
     * @return Vrai si la condition est remplie, faux sinon; aucun acces aux secrets.
     */
    [[nodiscard]] bool ok() const noexcept;
    /**
     * @brief Accede a l erreur uniquement lorsque ok() est faux; definir une politique de
     * precondition explicite.
     * @return Resultat de la lecture ou de l operation selon le type declare.
     * @pre ok() == false ; ne jamais lire cette branche sur un succes.
     */
    [[nodiscard]] const Error& error() const;
private:
    /**
     * @brief Erreur optionnelle; absence signifie succes pour Result<void>.
     */
    std::optional<Error> error_;
};

/** @brief Owned coroutine outcome; never carry borrowed spans over suspension. */
template <typename T>
/**
 * @brief Coroutine Boost.Asio transportant un Result<T>, pas une exception inter-module.
 */
using Async = boost::asio::awaitable<Result<T>>;

} // namespace ygg
