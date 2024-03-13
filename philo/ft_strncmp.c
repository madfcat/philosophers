/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vshchuki <vshchuki@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/24 18:34:26 by vshchuki          #+#    #+#             */
/*   Updated: 2024/03/14 00:23:43 by vshchuki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * The strncmp() function compares not more than n characters.
 * Because strncmp() is designed for comparing strings rather than binary data,
 * characters that appear after a '\0' character are not compared.
 * 
 * Returns an integer greater than, equal to, or less than 0, according as the
 * string s1 is greater than, equal to, or less than the string s2.
 * The comparison is done using unsigned characters, so that '\200' is greater
 * than '\0'.
*/
int	ft_strncmp(const char *s1, const char *s2, unsigned int n)
{
	unsigned int	i;

	i = 0;
	while (i < n)
	{
		if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0')
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
