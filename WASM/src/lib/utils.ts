/**
 * @module utils
 * @description Shared utility functions and TypeScript helper types used throughout the application.
 * Provides Tailwind CSS class merging via `cn()` and component prop utility types for
 * stripping `child`/`children` props and adding element refs — commonly needed when
 * wrapping headless UI primitives (bits-ui / shadcn-svelte).
 *
 * @see {@link https://github.com/lukeed/clsx} clsx — conditional class joining
 * @see {@link https://github.com/dcastil/tailwind-merge} tailwind-merge — deduplicates Tailwind classes
 */

import { clsx, type ClassValue } from "clsx";
import { twMerge } from "tailwind-merge";

/**
 * Merges and deduplicates Tailwind CSS class names.
 *
 * Combines multiple class values using `clsx` (handles conditionals, arrays, objects)
 * and then passes the result through `twMerge` to intelligently resolve conflicting
 * Tailwind utility classes (e.g. `p-2` vs `p-4`).
 *
 * @param inputs - Any number of class values (strings, arrays, objects, undefined, etc.)
 * @returns A single deduplicated class string
 *
 * @example
 * ```ts
 * cn("px-2 py-1", "px-4");          // "px-4 py-1"
 * cn("text-red-500", isActive && "font-bold"); // "text-red-500 font-bold" (when isActive)
 * cn({ "bg-blue-500": true, "bg-red-500": false }); // "bg-blue-500"
 * ```
 */
export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}

/**
 * Strips the `child` snippet prop from a component's prop type.
 * Useful when wrapping bits-ui primitives that expose a `child` render prop
 * but the wrapper component handles rendering internally.
 *
 * @template T - The original component props type
 *
 * @example
 * ```ts
 * type ButtonProps = WithoutChild<ButtonPrimitive.Props>;
 * ```
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export type WithoutChild<T> = T extends { child?: any } ? Omit<T, "child"> : T;

/**
 * Strips the `children` snippet prop from a component's prop type.
 * Used when a wrapper component provides its own children rendering logic
 * instead of delegating to the underlying primitive.
 *
 * @template T - The original component props type
 *
 * @example
 * ```ts
 * type CardProps = WithoutChildren<CardPrimitive.Props>;
 * ```
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
export type WithoutChildren<T> = T extends { children?: any }
  ? Omit<T, "children">
  : T;

/**
 * Strips both `child` and `children` snippet props from a component's prop type.
 * Combines {@link WithoutChild} and {@link WithoutChildren} for components that
 * need to omit both rendering delegation props.
 *
 * @template T - The original component props type
 */
export type WithoutChildrenOrChild<T> = WithoutChildren<WithoutChild<T>>;

/**
 * Extends a component's prop type with an optional `ref` property for direct
 * DOM element access. Defaults to `HTMLElement` but can be narrowed to a
 * specific element type.
 *
 * @template T - The original component props type
 * @template U - The HTML element type for the ref (defaults to `HTMLElement`)
 *
 * @example
 * ```ts
 * type InputProps = WithElementRef<InputPrimitive.Props, HTMLInputElement>;
 * // InputProps now includes: { ref?: HTMLInputElement | null }
 * ```
 */
export type WithElementRef<T, U extends HTMLElement = HTMLElement> = T & {
  ref?: U | null;
};
